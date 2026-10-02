#include "MainWindow.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "DateUtils.h"
#include "PageLayout.h"

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

    progressBody = new QLabel(page);
    progressBody->setObjectName("body");
    progressBody->setWordWrap(true);
    layout->addWidget(progressBody);
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

void MainWindow::showPeriod(const QString& period, const std::vector<std::string>& dates) {
    const PeriodSummary summary = habitService.periodSummary(currentUser->getId(), dates);

    if (summary.days == 0) {
        progressBody->setText("No days to show.");
        showPage(ProgressPage);
        return;
    }

    const int waterPercent = summary.waterGoalMl > 0
        ? summary.consumedWaterMl * 100 / summary.waterGoalMl
        : 0;
    const int sleepPercent = summary.sleepGoalHours > 0
        ? static_cast<int>(summary.sleepHours * 100.0 / summary.sleepGoalHours)
        : 0;

    // One day reads as a date, a longer period as a range.
    const QString heading = summary.days == 1
        ? period + " (" + QString::fromStdString(summary.firstDate) + ")"
        : period + " (" + QString::fromStdString(summary.firstDate) + " to "
              + QString::fromStdString(summary.lastDate) + ")";

    const QString exercise = summary.days == 1
        ? QString(summary.exerciseDays > 0 ? "Completed" : "Not completed")
        : QString::number(summary.exerciseDays) + " days completed";

    progressBody->setText(
        heading + "\n\n"
        + "Water:     " + QString::number(summary.consumedWaterMl) + " / "
            + QString::number(summary.waterGoalMl) + " ml ("
            + QString::number(waterPercent) + "%) - "
            + QString::number(summary.waterPoints) + " XP\n"
        + "Meals:     " + QString::number(summary.healthyMeals) + " healthy, "
            + QString::number(summary.unhealthyMeals) + " unhealthy - "
            + QString::number(summary.nutritionPoints) + " XP\n"
        + "Exercise:  " + exercise + " - "
            + QString::number(summary.exercisePoints) + " XP\n"
        + "Sleep:     " + QString::number(summary.sleepHours, 'f', 1) + " / "
            + QString::number(summary.sleepGoalHours, 'f', 1) + " hours ("
            + QString::number(sleepPercent) + "%) - "
            + QString::number(summary.sleepPoints) + " XP\n\n"
        + "Total XP:  " + QString::number(summary.totalPoints)
    );
    showPage(ProgressPage);
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
