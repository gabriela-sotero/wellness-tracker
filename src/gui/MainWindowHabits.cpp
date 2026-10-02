#include "MainWindow.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "PageLayout.h"

QWidget* MainWindow::createHabitsPage() {
    auto* page = new QWidget;
    auto* layout = startPage(page, "Log habits");

    waterInput = addField(layout, "Water in ml");
    auto* logWaterButton = new QPushButton("Log water", page);
    layout->addWidget(logWaterButton);

    layout->addWidget(new QLabel("Meal", page));
    auto* healthyMeal = new QPushButton("Log healthy meal", page);
    auto* unhealthyMeal = new QPushButton("Log unhealthy meal", page);
    layout->addWidget(healthyMeal);
    layout->addWidget(unhealthyMeal);

    layout->addWidget(new QLabel("Exercise", page));
    auto* exercise = new QPushButton("Mark exercise as completed", page);
    layout->addWidget(exercise);

    sleepInput = addField(layout, "Sleep in hours");
    auto* logSleepButton = new QPushButton("Log sleep", page);
    layout->addWidget(logSleepButton);

    habitFeedback = addFeedback(layout);

    auto* back = new QPushButton("Back", page);
    layout->addWidget(back);
    layout->addStretch();

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
    connect(back, &QPushButton::clicked, this, [this] {
        showPage(HomePage);
    });

    return page;
}

// Reports what was logged in green, and input problems in red.
static void report(QLabel* feedback, const QString& message, bool logged) {
    feedback->setStyleSheet(logged ? "color: #2e7d32;" : "");
    feedback->setText(message);
}

void MainWindow::logWater() {
    bool valid = false;
    const int ml = waterInput->text().trimmed().toInt(&valid);

    if (!valid || ml <= 0) {
        report(habitFeedback, "Enter a positive whole number in ml.", false);
        return;
    }

    habitService.logWaterHabit(currentUser->getId(), ml);
    waterInput->clear();
    report(habitFeedback, QString("Logged %1 ml of water.").arg(ml), true);
}

void MainWindow::logMeal(bool healthy) {
    habitService.logMealHabit(currentUser->getId(), healthy);
    report(
        habitFeedback,
        healthy ? "Logged a healthy meal." : "Logged an unhealthy meal.",
        true
    );
}

void MainWindow::logExercise() {
    habitService.logExerciseHabit(currentUser->getId());
    report(habitFeedback, "Exercise marked as completed.", true);
}

void MainWindow::logSleep() {
    bool valid = false;
    const double hours = sleepInput->text().trimmed().toDouble(&valid);

    if (!valid || hours <= 0.0) {
        report(habitFeedback, "Enter a positive number of hours.", false);
        return;
    }

    habitService.logSleepHabit(currentUser->getId(), hours);
    sleepInput->clear();
    report(habitFeedback, QString("Logged %1 hours of sleep.").arg(hours), true);
}
