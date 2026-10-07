#include "MainWindow.h"

#include <QHBoxLayout>
#include <QImageReader>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>

#include "Constants.h"
#include "DateUtils.h"
#include "PageLayout.h"

namespace {
const char* accountDialogStyle = R"(
    QDialog, QMessageBox, QInputDialog {
        background-color: #ffffff;
        color: #183b35;
    }
    QLabel { color: #183b35; background-color: transparent; }
    QLineEdit {
        background-color: #ffffff;
        color: #183b35;
        border: 1px solid #d5e2dd;
        border-radius: 6px;
        padding: 6px;
    }
    QSpinBox {
        background-color: #ffffff;
        color: #183b35;
        border: 1px solid #d5e2dd;
        border-radius: 6px;
        padding: 6px;
        selection-background-color: #007c68;
        selection-color: #ffffff;
    }
    QSpinBox::up-button, QSpinBox::down-button {
        background-color: #f4f7f5;
        border: none;
        border-left: 1px solid #d5e2dd;
        width: 22px;
    }
    QSpinBox::up-button {
        subcontrol-origin: border;
        subcontrol-position: top right;
    }
    QSpinBox::down-button {
        subcontrol-origin: border;
        subcontrol-position: bottom right;
    }
    QSpinBox::up-arrow {
        image: url(assets/icons/arrow-up.svg);
        width: 10px;
        height: 10px;
    }
    QSpinBox::down-arrow {
        image: url(assets/icons/arrow-down.svg);
        width: 10px;
        height: 10px;
    }
    QPushButton {
        background-color: #007c68;
        color: #ffffff;
        border: none;
        border-radius: 6px;
        padding: 7px 14px;
        min-width: 70px;
    }
    QPushButton:hover { background-color: #006454; }
)";

void showAccountMessage(
    QWidget* parent,
    QMessageBox::Icon icon,
    const QString& title,
    const QString& message
) {
    // Centralize dialog styling and execution for account-editing outcomes.
    QMessageBox dialog(icon, title, message, QMessageBox::Ok, parent);
    dialog.setStyleSheet(accountDialogStyle);
    dialog.exec();
}
}

// Adds a habit row, starting empty until a period is chosen.
static ProgressRow addProgressRow(QVBoxLayout* layout, const QString& name) {
    // Return widget pointers because later period selections update these controls.
    QWidget* page = layout->parentWidget();

    ProgressRow row;
    row.label = new QLabel(name, page);
    row.bar = new QProgressBar(page);
    layout->addWidget(row.label);
    layout->addWidget(row.bar);

    return row;
}

// Fills a row. The bar is clamped because QProgressBar ignores a value past
// its maximum, but the text keeps reporting what the user actually did.
static void fillProgressRow(
    const ProgressRow& row,
    const QString& name,
    const QString& rateDescription,
    int metDays,
    int totalDays,
    const QString& detail
) {
    // Long-period values show goal frequency; detail text preserves raw totals.
    const int percent = totalDays > 0 ? metDays * 100 / totalDays : 0;
    row.label->setText(
        name + " · " + rateDescription.arg(percent)
    );
    row.bar->setRange(0, totalDays > 0 ? totalDays : 1);
    row.bar->setValue(metDays);
    row.bar->setFormat(detail);
}

static void fillDailyProgressRow(
    const ProgressRow& row,
    const QString& label,
    int value,
    int maximum,
    const QString& detail
) {
    // A one-day view displays measured amounts rather than a percentage of days.
    row.label->setText(label);
    row.bar->setRange(0, maximum > 0 ? maximum : 1);
    row.bar->setValue(std::min(value, maximum));
    row.bar->setFormat(detail);
}

// Percent of a goal, reported past 100% when the goal is beaten.
static int percentOf(double value, double goal) {
    // A zero goal has no meaningful percentage and is reported as zero.
    return goal > 0 ? static_cast<int>(value * 100.0 / goal) : 0;
}

QWidget* MainWindow::createProgressPage() {
    // Build the reusable progress controls; period buttons supply data ranges.
    auto* page = new QWidget;
    auto* layout = startPage(page, "Progress", 560);

    // Segmented period selector. The button text doubles as the period name that
    // showPeriod receives, which is how the active tab is found.
    auto* tabs = new QHBoxLayout;
    tabs->setSpacing(8);
    auto* daily = new QPushButton("Today", page);
    auto* weekly = new QPushButton("Weekly", page);
    auto* monthly = new QPushButton("Monthly", page);
    auto* yearly = new QPushButton("Yearly", page);
    for (QPushButton* tab : {daily, weekly, monthly, yearly}) {
        tab->setObjectName("tab");
        tabs->addWidget(tab);
        periodTabs.push_back(tab);
    }
    layout->addLayout(tabs);

    progressHeading = new QLabel(page);
    progressHeading->setObjectName("heading");
    progressHeading->setWordWrap(true);
    layout->addWidget(progressHeading);

    auto* overall = addCard(layout, QString());
    progressOverall = new QLabel(page);
    progressOverall->setObjectName("value");
    overall->addWidget(progressOverall);
    progressOverallBar = new QProgressBar(page);
    progressOverallBar->setFormat("%v of %m daily goals met");
    overall->addWidget(progressOverallBar);

    // Each habit gets its own card; the rows are filled by showPeriod.
    waterRow = addProgressRow(addCard(layout, QString()), "Water");
    mealsRow = addProgressRow(addCard(layout, QString()), "Meals");
    exerciseRow = addProgressRow(addCard(layout, QString()), "Exercise");
    sleepRow = addProgressRow(addCard(layout, QString()), "Sleep");

    progressTotal = new QLabel(page);
    progressTotal->setObjectName("total");
    layout->addWidget(progressTotal);

    connect(daily, &QPushButton::clicked, this, [this] {
        showPeriod("Today", {util::today()});
    });
    connect(weekly, &QPushButton::clicked, this, [this] {
        showPeriod("Weekly", util::lastDays(7));
    });
    connect(monthly, &QPushButton::clicked, this, [this] {
        showPeriod("Monthly", util::monthToDate(), true);
    });
    connect(yearly, &QPushButton::clicked, this, [this] {
        showPeriod("Yearly", util::yearToDate(), true);
    });
    // Five cards are taller than a short window, so this page scrolls.
    return scrollPage(page);
}

void MainWindow::showPeriod(
    const QString& period,
    const std::vector<std::string>& dates,
    bool toDate
) {
    // The service owns aggregation; this method maps its summary into widgets.
    for (QPushButton* tab : periodTabs) {
        setActive(tab, tab->text() == period);
    }

    const PeriodSummary summary = habitService.periodSummary(currentUser->getId(), dates);

    if (summary.days == 0) {
        progressHeading->setText("No days to show.");
        showPage(ProgressPage);
        return;
    }

    // One day reads as a date. A longer period states how many days it covers,
    // so the denominator in each bar is not read as the whole month or year.
    if (summary.days == 1) {
        progressHeading->setText(
            period + " (" + QString::fromStdString(summary.firstDate) + ")"
        );
    } else {
        progressHeading->setText(
            period + " - " + QString::number(summary.days)
                + (toDate ? " days so far\n(" : " days\n(")
                + QString::fromStdString(summary.firstDate) + " to "
                + QString::fromStdString(summary.lastDate) + ")"
        );
    }

    if (summary.days == 1) {
        // A single day may be today or one picked from the home path's history.
        const QString when = period == "Today" ? QString("today") : QString("that day");
        progressOverall->setText(
            "Goals completed " + when + " · " + QString::number(summary.overallGoalsMet) + " / 4"
        );
        progressOverallBar->setRange(0, 4);
        progressOverallBar->setValue(summary.overallGoalsMet);

        fillDailyProgressRow(
            waterRow,
            "Water · " + QString::number(summary.consumedWaterMl) + " / "
                + QString::number(summary.waterGoalMl) + " ml",
            summary.consumedWaterMl,
            summary.waterGoalMl,
            QString::number(summary.consumedWaterMl) + " ml recorded"
        );
        fillDailyProgressRow(
            mealsRow,
            "Meals · " + QString::number(summary.healthyMeals) + " healthy, "
                + QString::number(summary.unhealthyMeals) + " unhealthy",
            summary.healthyMeals,
            Constants::HEALTHY_MEALS_GOAL,
            QString::number(summary.healthyMeals) + " / "
                + QString::number(Constants::HEALTHY_MEALS_GOAL) + " healthy meals"
        );
        fillDailyProgressRow(
            exerciseRow,
            summary.exerciseDays > 0 ? "Exercise · Completed" : "Exercise · Not completed",
            summary.exerciseDays,
            1,
            summary.exerciseDays > 0 ? "Completed " + when : "Not completed " + when
        );
        fillDailyProgressRow(
            sleepRow,
            "Sleep · " + QString::number(summary.sleepHours, 'f', 1) + " / "
                + QString::number(summary.sleepGoalHours, 'f', 1) + " h",
            static_cast<int>(summary.sleepHours * 10),
            static_cast<int>(summary.sleepGoalHours * 10),
            QString::number(summary.sleepHours, 'f', 1) + " hours recorded"
        );
        progressTotal->setText("Points earned " + when + ": " + QString::number(summary.totalPoints));
        showPage(ProgressPage);
        return;
    }

    const int goalOpportunities = summary.days * 4;
    const int overallPercent = percentOf(summary.overallGoalsMet, goalOpportunities);
    progressOverall->setText(
        "Overall habit consistency · " + QString::number(overallPercent) + "%"
    );
    progressOverallBar->setRange(0, goalOpportunities > 0 ? goalOpportunities : 1);
    progressOverallBar->setValue(summary.overallGoalsMet);

    const double averageWater =
        static_cast<double>(summary.consumedWaterMl) / summary.days;
    fillProgressRow(
        waterRow, "Water", "%1% of days at goal",
        summary.waterGoalDays, summary.days,
        QString::number(summary.waterGoalDays) + " of "
            + QString::number(summary.days) + " days · average "
            + QString::number(averageWater, 'f', 0) + " ml/day"
    );

    const double averageHealthyMeals =
        static_cast<double>(summary.healthyMeals) / summary.days;
    fillProgressRow(
        mealsRow, "Meals", "%1% of days at goal",
        summary.healthyMealsGoalDays, summary.days,
        QString::number(summary.healthyMealsGoalDays) + " of "
            + QString::number(summary.days) + " days at goal · average "
            + QString::number(averageHealthyMeals, 'f', 1) + " healthy/day"
    );

    fillProgressRow(
        exerciseRow, "Exercise", "%1% of days completed",
        summary.exerciseDays, summary.days,
        QString::number(summary.exerciseDays) + " of "
            + QString::number(summary.days) + " days completed"
    );

    const double averageSleep = summary.sleepHours / summary.days;
    fillProgressRow(
        sleepRow, "Sleep", "%1% of nights at 8h goal",
        summary.sleepGoalDays, summary.days,
        QString::number(summary.sleepGoalDays) + " of "
            + QString::number(summary.days) + " nights · average across period "
            + QString::number(averageSleep, 'f', 1) + " h/night"
    );

    progressTotal->setText(
        "Average daily score: "
            + QString::number(static_cast<double>(summary.totalPoints) / summary.days, 'f', 0)
            + " XP"
    );
    showPage(ProgressPage);
}

// Badge artwork ships as SVG under assets/badges, read relative to the working
// directory like the database is. Qt rasterises the file at the size asked for.
static QPixmap badgeArtwork(const QString& habit, int badgeDays, int width) {
    // The highest unlocked tier determines the SVG; zero selects the locked art.
    const QString file = badgeDays > 0
        ? habit + "-" + QString("%1").arg(badgeDays, 3, 10, QChar('0')) + ".svg"
        : QString("locked.svg");

    QImageReader reader("assets/badges/" + file);
    reader.setScaledSize(QSize(width, width * 86 / 70));
    return QPixmap::fromImage(reader.read());
}

QWidget* MainWindow::createProfilePage() {
    // Construct profile widgets once so showProfile can refresh the same controls.
    auto* page = new QWidget;
    auto* layout = startPage(page, "Profile", 560);

    auto* account = addCard(layout, QString());
    profileName = new QLabel(page);
    profileName->setObjectName("name");
    profileMemberSince = new QLabel(page);
    profileMemberSince->setObjectName("muted");
    account->addWidget(profileName);
    account->addWidget(profileMemberSince);

    auto* personalDetails = addCard(layout, "Personal details");
    auto* waterGoalRow = new QHBoxLayout;
    waterGoalRow->addWidget(new QLabel("Daily water goal", page));
    waterGoalRow->addStretch();
    profileWaterGoal = new QLabel(page);
    profileWaterGoal->setObjectName("value");
    waterGoalRow->addWidget(profileWaterGoal);
    auto* editWaterGoal = new QPushButton("Edit", page);
    editWaterGoal->setObjectName("secondary");
    editWaterGoal->setMinimumWidth(90);
    waterGoalRow->addWidget(editWaterGoal);
    personalDetails->addLayout(waterGoalRow);

    // Keep the weight row available so a person can add weight later as well.
    profileWeightRow = new QWidget(page);
    auto* weightRow = new QHBoxLayout(profileWeightRow);
    weightRow->setContentsMargins(0, 0, 0, 0);
    weightRow->addWidget(new QLabel("Weight", profileWeightRow));
    weightRow->addStretch();
    profileWeight = new QLabel(profileWeightRow);
    profileWeight->setObjectName("value");
    weightRow->addWidget(profileWeight);
    auto* editWeightButton = new QPushButton("Edit", profileWeightRow);
    editWeightButton->setObjectName("secondary");
    editWeightButton->setMinimumWidth(90);
    weightRow->addWidget(editWeightButton);
    personalDetails->addWidget(profileWeightRow);

    connect(editWaterGoal, &QPushButton::clicked, this, [this] {
        editDailyWaterGoal();
    });
    connect(editWeightButton, &QPushButton::clicked, this, [this] {
        editWeight();
    });

    auto* levelCard = addCard(layout, QString());
    levelProgressLabel = new QLabel(page);
    levelProgressLabel->setObjectName("value");
    levelProgressBar = new QProgressBar(page);
    levelProgressBar->setTextVisible(false);
    levelCard->addWidget(levelProgressLabel);
    levelCard->addWidget(levelProgressBar);

    auto* badges = addCard(layout, "Badges");
    auto* badgeRow = new QHBoxLayout;
    badgeRow->setContentsMargins(0, 4, 0, 0);
    for (const QString& habit : {"Water", "Meals", "Exercise"}) {
        auto* column = new QVBoxLayout;
        column->setSpacing(4);

        auto* image = new QLabel(page);
        image->setAlignment(Qt::AlignCenter);
        auto* name = new QLabel(habit, page);
        name->setObjectName("value");
        name->setAlignment(Qt::AlignCenter);
        auto* caption = new QLabel(page);
        caption->setObjectName("muted");
        caption->setAlignment(Qt::AlignCenter);

        column->addWidget(image);
        column->addWidget(name);
        column->addWidget(caption);
        badgeRow->addLayout(column);

        badgeImages.push_back(image);
        badgeCaptions.push_back(caption);
    }
    badges->addLayout(badgeRow);

    auto* xp = addCard(layout, "XP by habit");
    for (const QString& habit : {"Water", "Nutrition", "Exercise", "Sleep"}) {
        xpValues.push_back(addCardRow(xp, habit));
    }
    xpValues.push_back(addCardRow(xp, "Total"));
    xpValues.back()->setObjectName("total");

    auto* streaks = addCard(layout, "Current streaks");
    for (const QString& goal : {"Water goal met", "3 healthy meals",
                                "Exercise completed", "More than 8h sleep"}) {
        streakValues.push_back(addCardRow(streaks, goal));
    }

    auto* deleteAccount = new QPushButton("Delete account", page);
    deleteAccount->setObjectName("danger");
    layout->addWidget(deleteAccount);

    connect(deleteAccount, &QPushButton::clicked, this, [this] {
        if (!currentUser.has_value()) {
            return;
        }

        QMessageBox confirmation(
            QMessageBox::Warning,
            "Delete account",
            "Permanently delete your account and all habit data? This cannot be undone.",
            QMessageBox::Yes | QMessageBox::No,
            this
        );
        confirmation.setDefaultButton(QMessageBox::No);
        confirmation.setStyleSheet(accountDialogStyle);
        if (confirmation.exec() != QMessageBox::Yes) {
            return;
        }

        QInputDialog passwordDialog(this);
        passwordDialog.setWindowTitle("Confirm account deletion");
        passwordDialog.setLabelText("Enter your current password:");
        passwordDialog.setTextEchoMode(QLineEdit::Password);
        passwordDialog.setStyleSheet(accountDialogStyle);
        if (passwordDialog.exec() != QDialog::Accepted) {
            return;
        }
        const QString password = passwordDialog.textValue();

        if (!database.authenticate(
                currentUser->getUsername(), password.toStdString()).has_value()) {
            showAccountMessage(this, QMessageBox::Critical, "Delete account",
                               "Incorrect password. Account was not deleted.");
            return;
        }

        if (!database.deleteUser(currentUser->getId())) {
            showAccountMessage(this, QMessageBox::Critical, "Delete account",
                               "Could not delete the account. Please try again.");
            return;
        }

        logOut();
        showAccountMessage(this, QMessageBox::Information, "Delete account",
                           "Your account and associated data were deleted.");
    });

    // The profile cards are taller than a short window, so this page scrolls.
    return scrollPage(page);
}

void MainWindow::showProfile() {
    // Read model and summary values, then render them without recalculating rules.
    const int userId = currentUser->getId();
    const ProfileSummary profile = habitService.profileSummary(userId);
    const auto createdAt = database.accountCreatedAt(userId);

    profileName->setText(QString::fromStdString(currentUser->getName()));
    profileMemberSince->setText(
        createdAt.has_value()
            ? "Member since " + QString::fromStdString(*createdAt)
            : QString("Created before the app recorded a date")
    );
    profileWaterGoal->setText(
        QString::number(currentUser->getWaterGoalMl()) + " ml/day"
    );
    const auto& weightKg = currentUser->getWeightKg();
    profileWeight->setText(
        weightKg.has_value()
            ? QString::number(weightKg.value(), 'g', 4) + " kg"
            : QString("Not provided")
    );

    const LevelProgress level = profile.levelProgress;
    levelProgressLabel->setText(
        QString("Level %1 — %2 / %3 XP")
            .arg(level.level)
            .arg(level.xpIntoLevel)
            .arg(level.xpForNextLevel)
    );
    levelProgressBar->setRange(0, level.xpForNextLevel);
    levelProgressBar->setValue(level.xpIntoLevel);

    const QString habits[] = {"water", "meals", "exercise"};
    const int best[] = {
        profile.waterBestStreak,
        profile.healthyMealsBestStreak,
        profile.exerciseBestStreak
    };
    for (int habit = 0; habit < 3; ++habit) {
        const int badgeDays = badgeDaysFor(best[habit]);
        badgeImages[habit]->setPixmap(badgeArtwork(habits[habit], badgeDays, 80));
        badgeCaptions[habit]->setText(
            badgeDays > 0
                ? QString("best %1 days").arg(best[habit])
                : QString("no badge yet")
        );
    }

    xpValues[0]->setText(QString::number(profile.waterXp) + " XP");
    xpValues[1]->setText(QString::number(profile.nutritionXp) + " XP");
    xpValues[2]->setText(QString::number(profile.exerciseXp) + " XP");
    xpValues[3]->setText(QString::number(profile.sleepXp) + " XP");
    xpValues[4]->setText(QString::number(profile.totalXp) + " XP");

    streakValues[0]->setText(QString::number(profile.waterStreak) + " days");
    streakValues[1]->setText(QString::number(profile.healthyMealsStreak) + " days");
    streakValues[2]->setText(QString::number(profile.exerciseStreak) + " days");
    streakValues[3]->setText(QString::number(profile.sleepStreak) + " days");

    showPage(ProfilePage);
}

void MainWindow::editDailyWaterGoal() {
    // Persist the edited goal while retaining the optional weight unchanged.
    if (!currentUser.has_value()) {
        return;
    }

    QInputDialog dialog(this);
    dialog.setWindowTitle("Daily water goal");
    dialog.setLabelText("Daily water goal in ml:");
    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setIntRange(1, 100000);
    dialog.setIntStep(250);
    dialog.setIntValue(currentUser->getWaterGoalMl());
    dialog.setStyleSheet(accountDialogStyle);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const int userId = currentUser->getId();
    if (!database.updateUserMetrics(
            userId, currentUser->getWeightKg(), dialog.intValue())) {
        showAccountMessage(this, QMessageBox::Critical, "Daily water goal",
                           "Could not update your daily water goal. Please try again.");
        return;
    }
    currentUser = database.getUserById(userId);
    if (!currentUser.has_value()) {
        showAccountMessage(this, QMessageBox::Critical, "Daily water goal",
                           "Could not reload your profile. Please sign in again.");
        return;
    }
    showProfile();
}

void MainWindow::editWeight() {
    // Persist an entered weight while retaining the current water goal.
    if (!currentUser.has_value()) {
        return;
    }

    QInputDialog dialog(this);
    dialog.setWindowTitle("Edit weight");
    dialog.setLabelText("Weight in kg:");
    dialog.setInputMode(QInputDialog::TextInput);
    if (currentUser->getWeightKg().has_value()) {
        dialog.setTextValue(QString::number(currentUser->getWeightKg().value(), 'g', 4));
    }
    dialog.setStyleSheet(accountDialogStyle);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    bool valid = false;
    const double weightKg = dialog.textValue().trimmed().toDouble(&valid);
    if (!valid || weightKg < 1.0 || weightKg > 500.0) {
        showAccountMessage(this, QMessageBox::Warning, "Edit weight",
                           "Enter a weight between 1 and 500 kg.");
        return;
    }

    const int userId = currentUser->getId();
    if (!database.updateUserMetrics(
            userId, weightKg, currentUser->getWaterGoalMl())) {
        showAccountMessage(this, QMessageBox::Critical, "Edit weight",
                           "Could not update your weight. Please try again.");
        return;
    }
    currentUser = database.getUserById(userId);
    if (!currentUser.has_value()) {
        showAccountMessage(this, QMessageBox::Critical, "Edit weight",
                           "Could not reload your profile. Please sign in again.");
        return;
    }
    showProfile();
}
