#include "MainWindow.h"

#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <utility>
#include <vector>

#include "Constants.h"
#include "DateUtils.h"
#include "Icons.h"
#include "PageLayout.h"

namespace {
struct Snapshot { int level; int xp; int goals; };

Snapshot snapshot(HabitService& service, int id) {
    const auto profile = service.profileSummary(id);
    const auto today = service.periodSummary(id, {util::today()});
    return {static_cast<int>(profile.levelProgress.level), static_cast<int>(profile.totalXp),
            static_cast<int>(today.overallGoalsMet)};
}

void feedback(QLabel* label, const QString& text, bool success) {
    label->setStyleSheet(success
        ? "background:#e0f2ee;color:#005c4d;border:2px solid #8fcfc3;border-radius:12px;padding:8px 12px;font-weight:800;"
        : "background:#fdf2f2;color:#d64545;border:2px solid #f1d0d0;border-radius:12px;padding:8px 12px;font-weight:800;");
    label->setText(text);
    label->setVisible(!text.isEmpty());
}

QuestCard card(QVBoxLayout* layout, const char* icon, const QColor& color,
               const QColor& tint, const QString& title) {
    QuestCard q;
    q.body = addCard(layout, QString());
    q.body->setSpacing(10);
    q.card = q.body->parentWidget();
    auto* row = new QHBoxLayout;
    row->setSpacing(12);
    auto* image = new QLabel(q.card);
    image->setFixedSize(46, 46);
    image->setAlignment(Qt::AlignCenter);
    image->setStyleSheet(QString("background:%1;border-radius:14px;").arg(tint.name()));
    image->setPixmap(icons::svgPixmap(icon, color, 26));
    row->addWidget(image);
    auto* words = new QVBoxLayout;
    words->setSpacing(0);
    auto* name = new QLabel(title, q.card);
    name->setObjectName("cardTitle");
    q.status = new QLabel(q.card);
    q.status->setObjectName("questStatus");
    words->addWidget(name);
    words->addWidget(q.status);
    row->addLayout(words, 1);
    q.badge = new QLabel(q.card);
    q.badge->setObjectName("doneBadge");
    q.badge->setFixedSize(26, 26);
    q.badge->setAlignment(Qt::AlignCenter);
    q.badge->setPixmap(icons::svgPixmap(icons::check, QColor("#ffffff"), 16));
    q.badge->hide();
    row->addWidget(q.badge);
    q.body->addLayout(row);
    q.bar = new QProgressBar(q.card);
    q.bar->setTextVisible(false);
    q.bar->setStyleSheet(QString("QProgressBar{min-height:14px;max-height:14px;border-radius:7px;}QProgressBar::chunk{background:%1;border-radius:7px;}").arg(color.name()));
    q.body->addWidget(q.bar);
    q.feedback = new QLabel(q.card);
    q.feedback->setWordWrap(true);
    q.feedback->hide();
    q.body->addWidget(q.feedback);
    return q;
}

void quickButtons(QVBoxLayout* body, QWidget* parent,
                  const std::vector<std::pair<QString,double>>& options,
                  const std::function<void(double)>& action) {
    auto* row = new QHBoxLayout;
    row->setSpacing(8);
    for (const auto& option : options) {
        auto* button = new QPushButton(option.first, parent);
        button->setObjectName("secondary");
        button->setCursor(Qt::PointingHandCursor);
        row->addWidget(button);
        const double value = option.second;
        QObject::connect(button, &QPushButton::clicked, button, [action, value] { action(value); });
    }
    body->addLayout(row);
}

QLineEdit* customEntry(QVBoxLayout* body, QWidget* parent, const QString& placeholder,
                       QPushButton*& button) {
    auto* row = new QHBoxLayout;
    row->setSpacing(8);
    auto* field = new QLineEdit(parent);
    field->setPlaceholderText(placeholder);
    field->setAccessibleName(placeholder);
    button = new QPushButton("Log", parent);
    row->addWidget(field, 1);
    row->addWidget(button);
    body->addLayout(row);
    return field;
}
}

QWidget* MainWindow::createHabitsPage() {
    auto* page = new QWidget;
    auto* layout = startPage(page, "Daily quests");
    auto* overview = addCard(layout, QString());
    questsSummary = new QLabel(page);
    questsSummary->setObjectName("heading");
    auto* hint = new QLabel("Meet all four goals to complete today's step on your path.", page);
    hint->setObjectName("muted");
    hint->setWordWrap(true);
    questsBar = new QProgressBar(page);
    questsBar->setTextVisible(false);
    questsBar->setRange(0, 4);
    questsBar->setStyleSheet("QProgressBar{min-height:18px;max-height:18px;border-radius:9px;}QProgressBar::chunk{background:#007c68;border-radius:9px;}");
    overview->addWidget(questsSummary);
    overview->addWidget(hint);
    overview->addWidget(questsBar);

    waterQuest = card(layout, icons::drop, QColor("#007c68"), QColor("#e0f2ee"), "Water");
    quickButtons(waterQuest.body, page, {{"+250 ml",250},{"+500 ml",500},{"+750 ml",750}},
                 [this](double ml) { addWater(static_cast<int>(ml)); });
    QPushButton* waterButton = nullptr;
    waterInput = customEntry(waterQuest.body, page, "Other amount in ml", waterButton);

    mealQuest = card(layout, icons::meal, QColor("#007c68"), QColor("#e0f2ee"), "Meals");
    auto* meals = new QHBoxLayout;
    auto* healthy = new QPushButton("Healthy meal", page);
    auto* unhealthy = new QPushButton("Unhealthy", page);
    unhealthy->setObjectName("secondary");
    meals->addWidget(healthy);
    meals->addWidget(unhealthy);
    mealQuest.body->addLayout(meals);

    exerciseQuest = card(layout, icons::dumbbell, QColor("#007c68"), QColor("#e0f2ee"), "Exercise");
    exerciseButton = new QPushButton("Mark as completed", page);
    exerciseQuest.body->addWidget(exerciseButton);

    sleepQuest = card(layout, icons::moon, QColor("#007c68"), QColor("#e0f2ee"), "Sleep");
    quickButtons(sleepQuest.body, page, {{"6 h",6},{"7 h",7},{"8 h",8},{"9 h",9}},
                 [this](double hours) { addSleep(hours); });
    QPushButton* sleepButton = nullptr;
    sleepInput = customEntry(sleepQuest.body, page, "Other hours (for example 7.5)", sleepButton);

    connect(waterButton, &QPushButton::clicked, this, [this] { logWater(); });
    connect(waterInput, &QLineEdit::returnPressed, this, [this] { logWater(); });
    connect(healthy, &QPushButton::clicked, this, [this] { logMeal(true); });
    connect(unhealthy, &QPushButton::clicked, this, [this] { logMeal(false); });
    connect(exerciseButton, &QPushButton::clicked, this, [this] { logExercise(); });
    connect(sleepButton, &QPushButton::clicked, this, [this] { logSleep(); });
    connect(sleepInput, &QLineEdit::returnPressed, this, [this] { logSleep(); });
    return scrollPage(page);
}

void MainWindow::refreshHabits() {
    if (!currentUser) return;
    const auto today = habitService.periodSummary(currentUser->getId(), {util::today()});
    if (today.days == 0) return;
    auto fill = [](QuestCard& q, bool done, const QString& status, int value, int maximum) {
        const int top = std::max(1, maximum);
        q.status->setText(status);
        q.bar->setRange(0, top);
        q.bar->setValue(std::min(value, top));
        setActive(q.card, done);
        q.badge->setVisible(done);
    };
    const int met = static_cast<int>(today.overallGoalsMet);
    questsSummary->setText(met >= 4 ? "All 4 goals complete!" : QString("%1 of 4 goals today").arg(met));
    questsBar->setValue(met);
    fill(waterQuest, today.waterGoalDays > 0, QString("%1 / %2 ml").arg(today.consumedWaterMl).arg(today.waterGoalMl), today.consumedWaterMl, today.waterGoalMl);
    fill(mealQuest, today.healthyMealsGoalDays > 0, QString("%1 / %2 healthy · %3 unhealthy").arg(today.healthyMeals).arg(Constants::HEALTHY_MEALS_GOAL).arg(today.unhealthyMeals), today.healthyMeals, Constants::HEALTHY_MEALS_GOAL);
    const bool exerciseDone = today.exerciseDays > 0;
    fill(exerciseQuest, exerciseDone, exerciseDone ? "Completed today" : "Not completed yet", exerciseDone ? 1 : 0, 1);
    exerciseButton->setEnabled(!exerciseDone);
    exerciseButton->setText(exerciseDone ? "Completed today" : "Mark as completed");
    fill(sleepQuest, today.sleepGoalDays > 0, QString("%1 / %2 h").arg(today.sleepHours,0,'f',1).arg(today.sleepGoalHours,0,'f',1), static_cast<int>(today.sleepHours*10), static_cast<int>(today.sleepGoalHours*10));
}

void MainWindow::logHabit(QuestCard& quest, const QString& message, const std::function<void()>& record) {
    const int id = currentUser->getId();
    const Snapshot before = snapshot(habitService, id);
    record();
    const Snapshot after = snapshot(habitService, id);
    refreshHabits();
    QStringList rewards;
    if (after.xp > before.xp) rewards << QString("+%1 XP").arg(after.xp-before.xp);
    if (after.goals > before.goals) rewards << "Goal complete!";
    if (after.level > before.level) rewards << QString("Level up! You reached level %1.").arg(after.level);
    feedback(quest.feedback, rewards.isEmpty() ? message : message + "\n" + rewards.join("  ·  "), true);
}

void MainWindow::addWater(int ml) {
    logHabit(waterQuest, QString("Logged %1 ml of water.").arg(ml), [this, ml] { habitService.logWaterHabit(currentUser->getId(), ml); });
}
void MainWindow::addSleep(double hours) {
    logHabit(sleepQuest, QString("Logged %1 hours of sleep.").arg(hours), [this, hours] { habitService.logSleepHabit(currentUser->getId(), hours); });
}
void MainWindow::logWater() {
    bool valid = false;
    const int ml = waterInput->text().trimmed().toInt(&valid);
    if (!valid || ml <= 0) { feedback(waterQuest.feedback, "Enter a positive whole number in ml.", false); return; }
    addWater(ml);
    waterInput->clear();
}
void MainWindow::logMeal(bool healthy) {
    logHabit(mealQuest, healthy ? "Logged a healthy meal." : "Logged an unhealthy meal.", [this, healthy] { habitService.logMealHabit(currentUser->getId(), healthy); });
}
void MainWindow::logExercise() {
    logHabit(exerciseQuest, "Exercise marked as completed.", [this] { habitService.logExerciseHabit(currentUser->getId()); });
}
void MainWindow::logSleep() {
    bool valid = false;
    const double hours = sleepInput->text().trimmed().toDouble(&valid);
    if (!valid || hours <= 0.0) { feedback(sleepQuest.feedback, "Enter a positive number of hours.", false); return; }
    addSleep(hours);
    sleepInput->clear();
}
