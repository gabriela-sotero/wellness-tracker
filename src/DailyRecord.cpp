#include "DailyRecord.h"

// Construct the day by composing one concrete object for each habit domain.
DailyRecord::DailyRecord(
    int userId,
    std::string date,
    int dailyGoalMl
):
    water(dailyGoalMl),
    nutrition(3),
    exercise(),
    sleep(),
    date(date),
    userId(userId) {
}

void DailyRecord::logWater(int ml) {
    // Keep validation and scoring rules inside WaterHabit.
    water.drankWater(ml);
    return;
}

void DailyRecord::logMeal(bool healthy) {
    // The bool chooses which operation of the composed NutritionHabit to call.
    if (healthy) {
        nutrition.logHealthyMeal();
    } else {
        nutrition.logUnhealthyMeal();
    }
}

void DailyRecord::seedMeals(int healthyMeals, int unhealthyMeals) {
    // Rebuild persisted counts through the public model operations so the same
    // in-memory invariants apply to restored data.
    for (int i = 0; i < healthyMeals; ++i) {
        nutrition.logHealthyMeal();
    }
    for (int i = 0; i < unhealthyMeals; ++i) {
        nutrition.logUnhealthyMeal();
    }
}

void DailyRecord::logExercise() {
    exercise.markCompleted();
}

void DailyRecord::seedExercise(bool completed) {
    // Only completed exercise needs an explicit action; false is the default.
    if (completed) {
        exercise.markCompleted();
    }
}

void DailyRecord::logSleep(double hours) {
    sleep.logSleep(hours);
}

void DailyRecord::seedSleep(double hours) {
    sleep.logSleep(hours);
}

int DailyRecord::dailyScore() const{
    // Ask each composed habit to calculate its score, then add the four results.
    return water.calculateScore() + nutrition.calculateScore()
        + exercise.calculateScore() + sleep.calculateScore();
}

int DailyRecord::nutritionScore() const {
    return nutrition.calculateScore();
}

int DailyRecord::exerciseScore() const {
    return exercise.calculateScore();
}

bool DailyRecord::exerciseCompleted() const {
    return exercise.isCompleted();
}

double DailyRecord::sleptHours() const {
    return sleep.getSleptHours();
}

int DailyRecord::sleepScore() const {
    return sleep.calculateScore();
}

int DailyRecord::waterScore() const {
    return water.calculateScore();
}

int DailyRecord::consumedWaterMl() const {
    return water.getConsumedMl();
}

int DailyRecord::healthyMealCount() const {
    return nutrition.healthyMealCount();
}

int DailyRecord::unhealthyMealCount() const {
    return nutrition.unhealthyMealCount();
}

int DailyRecord::getUserId() const {
    return userId;
}

const std::string& DailyRecord::getDate() const {
    return date;
}
