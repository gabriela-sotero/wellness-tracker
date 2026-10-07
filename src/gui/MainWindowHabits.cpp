#include "MainWindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "PageLayout.h"

QWidget* MainWindow::createHabitsPage() {
    // One card per habit; Qt signals connect to focused logging methods below.
    // Navigation lives in the bottom bar, so this page has no Back button.
    auto* page = new QWidget;
    auto* layout = startPage(page, "Log habits");

    auto* water = addCard(layout, "Water");
    waterInput = addField(water, "Amount in ml");
    auto* logWaterButton = new QPushButton("Log water", page);
    water->addWidget(logWaterButton);

    auto* meal = addCard(layout, "Meal");
    auto* mealRow = new QHBoxLayout;
    mealRow->setSpacing(10);
    auto* healthyMeal = new QPushButton("Healthy", page);
    auto* unhealthyMeal = new QPushButton("Unhealthy", page);
    unhealthyMeal->setObjectName("secondary");
    mealRow->addWidget(healthyMeal);
    mealRow->addWidget(unhealthyMeal);
    meal->addLayout(mealRow);

    auto* exerciseCard = addCard(layout, "Exercise");
    auto* exercise = new QPushButton("Mark as completed", page);
    exerciseCard->addWidget(exercise);

    auto* sleep = addCard(layout, "Sleep");
    sleepInput = addField(sleep, "Hours slept");
    auto* logSleepButton = new QPushButton("Log sleep", page);
    sleep->addWidget(logSleepButton);

    habitFeedback = addFeedback(layout);

    connect(logWaterButton, &QPushButton::clicked, this, [this] {
        logWater();
    });
    connect(waterInput, &QLineEdit::returnPressed, this, [this] {
        logWater();
    });
    connect(healthyMeal, &QPushButton::clicked, this, [this] {
        logMeal(true);
    });
    connect(unhealthyMeal, &QPushButton::clicked, this, [this] {
        logMeal(false);
    });
    connect(exercise, &QPushButton::clicked, this, [this] {
        logExercise();
    });
    connect(logSleepButton, &QPushButton::clicked, this, [this] {
        logSleep();
    });
    connect(sleepInput, &QLineEdit::returnPressed, this, [this] {
        logSleep();
    });

    // Four cards are taller than a short window, so this page scrolls.
    return scrollPage(page);
}

// Reports what was logged in green, and input problems in red.
static void report(QLabel* feedback, const QString& message, bool logged) {
    // A shared presenter keeps success and validation feedback visually consistent.
    feedback->setStyleSheet(logged ? "color: #007c68;" : "");
    feedback->setText(message);
}

QString MainWindow::levelUpMessage(int previousLevel) const {
    // Re-read calculated profile level after a save and report only increases.
    const int newLevel = habitService.profileSummary(currentUser->getId())
        .levelProgress.level;
    return newLevel > previousLevel
        ? QString("\nLevel up! You reached level %1.").arg(newLevel)
        : QString();
}

void MainWindow::logWater() {
    // Convert text input to a positive integer before delegating to the service.
    bool valid = false;
    const int ml = waterInput->text().trimmed().toInt(&valid);

    if (!valid || ml <= 0) {
        report(habitFeedback, "Enter a positive whole number in ml.", false);
        return;
    }

    const int previousLevel = habitService.profileSummary(currentUser->getId())
        .levelProgress.level;
    habitService.logWaterHabit(currentUser->getId(), ml);
    waterInput->clear();
    report(habitFeedback,
           QString("Logged %1 ml of water.").arg(ml) + levelUpMessage(previousLevel), true);
}

void MainWindow::logMeal(bool healthy) {
    // The button supplies the meal category; HabitService owns recording rules.
    const int previousLevel = habitService.profileSummary(currentUser->getId())
        .levelProgress.level;
    habitService.logMealHabit(currentUser->getId(), healthy);
    report(
        habitFeedback,
        (healthy ? QString("Logged a healthy meal.") : QString("Logged an unhealthy meal."))
            + levelUpMessage(previousLevel),
        true
    );
}

void MainWindow::logExercise() {
    // Exercise has no numeric input: the action marks today's binary goal complete.
    const int previousLevel = habitService.profileSummary(currentUser->getId())
        .levelProgress.level;
    habitService.logExerciseHabit(currentUser->getId());
    report(habitFeedback,
           QString("Exercise marked as completed.") + levelUpMessage(previousLevel), true);
}

void MainWindow::logSleep() {
    // Convert and validate the entered duration before recording it.
    bool valid = false;
    const double hours = sleepInput->text().trimmed().toDouble(&valid);

    if (!valid || hours <= 0.0) {
        report(habitFeedback, "Enter a positive number of hours.", false);
        return;
    }

    const int previousLevel = habitService.profileSummary(currentUser->getId())
        .levelProgress.level;
    habitService.logSleepHabit(currentUser->getId(), hours);
    sleepInput->clear();
    report(habitFeedback,
           QString("Logged %1 hours of sleep.").arg(hours) + levelUpMessage(previousLevel), true);
}
