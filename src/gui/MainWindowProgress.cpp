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
    QMessageBox dialog(icon, title, message, QMessageBox::Ok, parent);
    dialog.setStyleSheet(accountDialogStyle);
    dialog.exec();
}
}

// Adds a habit row, starting empty until a period is chosen.
static ProgressRow addProgressRow(QVBoxLayout* layout, const QString& name) {
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
    int points,
    int value,
    int maximum,
    const QString& text
) {
    row.label->setText(name + " - " + QString::number(points) + " XP");
    row.bar->setMaximum(maximum > 0 ? maximum : 1);
    row.bar->setValue(value < maximum ? value : maximum);
    row.bar->setFormat(text);
}

// Percent of a goal, reported past 100% when the goal is beaten.
static int percentOf(double value, double goal) {
    return goal > 0 ? static_cast<int>(value * 100.0 / goal) : 0;
}

QWidget* MainWindow::createProgressPage() {
    auto* page = new QWidget;
    auto* layout = startPage(page, "Progress");

    auto* daily = new QPushButton("Today", page);
    auto* weekly = new QPushButton("Weekly", page);
    auto* monthly = new QPushButton("Monthly", page);
    auto* yearly = new QPushButton("Yearly", page);
    layout->addWidget(daily);
    layout->addWidget(weekly);
    layout->addWidget(monthly);
    layout->addWidget(yearly);

    progressHeading = new QLabel(page);
    progressHeading->setWordWrap(true);
    layout->addWidget(progressHeading);

    waterRow = addProgressRow(layout, "Water");
    mealsRow = addProgressRow(layout, "Meals");
    exerciseRow = addProgressRow(layout, "Exercise");
    sleepRow = addProgressRow(layout, "Sleep");

    progressTotal = new QLabel(page);
    layout->addWidget(progressTotal);
    layout->addStretch();

    auto* back = new QPushButton("Back", page);
    layout->addWidget(back);

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
    connect(back, &QPushButton::clicked, this, [this] {
        showPage(HomePage);
    });
    return page;
}

void MainWindow::showPeriod(
    const QString& period,
    const std::vector<std::string>& dates,
    bool toDate
) {
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

    const int waterPercent = percentOf(summary.consumedWaterMl, summary.waterGoalMl);
    fillProgressRow(
        waterRow, "Water", summary.waterPoints,
        summary.consumedWaterMl, summary.waterGoalMl,
        QString::number(summary.consumedWaterMl) + " / "
            + QString::number(summary.waterGoalMl) + " ml ("
            + QString::number(waterPercent) + "%)"
    );

    const int mealsGoal = Constants::HEALTHY_MEALS_GOAL * summary.days;
    const int mealsPercent = percentOf(summary.healthyMeals, mealsGoal);
    fillProgressRow(
        mealsRow, "Meals", summary.nutritionPoints,
        summary.healthyMeals, mealsGoal,
        QString::number(summary.healthyMeals) + " / " + QString::number(mealsGoal)
            + " healthy, " + QString::number(summary.unhealthyMeals)
            + " unhealthy (" + QString::number(mealsPercent) + "%)"
    );

    const int exercisePercent = percentOf(summary.exerciseDays, summary.days);
    fillProgressRow(
        exerciseRow, "Exercise", summary.exercisePoints,
        summary.exerciseDays, summary.days,
        summary.days == 1
            ? QString(summary.exerciseDays > 0 ? "Completed" : "Not completed")
            : QString::number(summary.exerciseDays) + " / "
                  + QString::number(summary.days) + " days ("
                  + QString::number(exercisePercent) + "%)"
    );

    // Tenths of an hour keep the bar accurate, since its value is an integer.
    const int sleepPercent = percentOf(summary.sleepHours, summary.sleepGoalHours);
    fillProgressRow(
        sleepRow, "Sleep", summary.sleepPoints,
        static_cast<int>(summary.sleepHours * 10),
        static_cast<int>(summary.sleepGoalHours * 10),
        QString::number(summary.sleepHours, 'f', 1) + " / "
            + QString::number(summary.sleepGoalHours, 'f', 1) + " hours ("
            + QString::number(sleepPercent) + "%)"
    );

    progressTotal->setText("Total XP: " + QString::number(summary.totalPoints));
    showPage(ProgressPage);
}

// Badge artwork ships as SVG under assets/badges, read relative to the working
// directory like the database is. Qt rasterises the file at the size asked for.
static QPixmap badgeArtwork(const QString& habit, int badgeDays, int width) {
    const QString file = badgeDays > 0
        ? habit + "-" + QString("%1").arg(badgeDays, 3, 10, QChar('0')) + ".svg"
        : QString("locked.svg");

    QImageReader reader("assets/badges/" + file);
    reader.setScaledSize(QSize(width, width * 86 / 70));
    return QPixmap::fromImage(reader.read());
}

QWidget* MainWindow::createProfilePage() {
    auto* page = new QWidget;
    auto* layout = startPage(page, "Profile");

    auto* account = addCard(layout, QString());
    profileName = new QLabel(page);
    profileName->setObjectName("name");
    profileMemberSince = new QLabel(page);
    profileMemberSince->setObjectName("muted");
    account->addWidget(profileName);
    account->addWidget(profileMemberSince);

    auto* levelCard = addCard(layout, "Level");
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

    layout->addStretch();
    auto* back = new QPushButton("Back", page);
    auto* deleteAccount = new QPushButton("Delete account", page);
    deleteAccount->setStyleSheet("color: #a32121;");
    layout->addWidget(back);
    layout->addWidget(deleteAccount);

    connect(back, &QPushButton::clicked, this, [this] {
        showPage(HomePage);
    });
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

    return page;
}

void MainWindow::showProfile() {
    const int userId = currentUser->getId();
    const ProfileSummary profile = habitService.profileSummary(userId);
    const auto createdAt = database.accountCreatedAt(userId);

    profileName->setText(QString::fromStdString(currentUser->getName()));
    profileMemberSince->setText(
        createdAt.has_value()
            ? "Member since " + QString::fromStdString(*createdAt)
            : QString("Created before the app recorded a date")
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
