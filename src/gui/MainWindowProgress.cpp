#include "MainWindow.h"

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "Constants.h"
#include "DateUtils.h"
#include "PageLayout.h"

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
        showPeriod("Monthly", util::monthToDate());
    });
    connect(yearly, &QPushButton::clicked, this, [this] {
        showPeriod("Yearly", util::yearToDate());
    });
    connect(back, &QPushButton::clicked, this, [this] {
        showPage(HomePage);
    });

    return page;
}

void MainWindow::showPeriod(const QString& period, const std::vector<std::string>& dates) {
    const PeriodSummary summary = habitService.periodSummary(currentUser->getId(), dates);

    if (summary.days == 0) {
        progressHeading->setText("No days to show.");
        showPage(ProgressPage);
        return;
    }

    // One day reads as a date, a longer period as a range.
    progressHeading->setText(
        summary.days == 1
            ? period + " (" + QString::fromStdString(summary.firstDate) + ")"
            : period + " (" + QString::fromStdString(summary.firstDate) + " to "
                  + QString::fromStdString(summary.lastDate) + ")"
    );

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

QWidget* MainWindow::createProfilePage() {
    auto* page = new QWidget;
    auto* layout = startPage(page, "Profile");

    profileBody = new QLabel(page);
    profileBody->setObjectName("body");
    profileBody->setWordWrap(true);
    layout->addWidget(profileBody);
    layout->addStretch();

    auto* back = new QPushButton("Back", page);
    layout->addWidget(back);

    connect(back, &QPushButton::clicked, this, [this] {
        showPage(HomePage);
    });

    return page;
}

void MainWindow::showProfile() {
    const ProfileSummary profile = habitService.profileSummary(currentUser->getId());

    profileBody->setText(
        QString::fromStdString(currentUser->getName()) + "\n\n"
        + "XP by habit\n"
        + "Water:     " + QString::number(profile.waterXp) + " XP\n"
        + "Nutrition: " + QString::number(profile.nutritionXp) + " XP\n"
        + "Exercise:  " + QString::number(profile.exerciseXp) + " XP\n"
        + "Sleep:     " + QString::number(profile.sleepXp) + " XP\n"
        + "Total:     " + QString::number(profile.totalXp) + " XP\n\n"
        + "Current goal streaks (days)\n"
        + "Water goal met:     " + QString::number(profile.waterStreak) + "\n"
        + "3 healthy meals:    " + QString::number(profile.healthyMealsStreak) + "\n"
        + "Exercise completed: " + QString::number(profile.exerciseStreak) + "\n"
        + "More than 8h sleep: " + QString::number(profile.sleepStreak)
    );
    showPage(ProfilePage);
}
