#include "DailyRecord.h"

DailyRecord::DailyRecord(
    int userId,
    std::string date,
    int dailyGoalMl
):
    water(dailyGoalMl),
    nutrition(3),
    exercise(),
    date(date),
    userId(userId) {
}

void DailyRecord::logWater(int ml) {
    water.drankWater(ml);
    return;
}

void DailyRecord::logMeal(bool healthy) {
    if (healthy) {
        nutrition.logHealthyMeal();
    } else {
        nutrition.logUnhealthyMeal();
    }
}

void DailyRecord::seedMeals(int healthyMeals, int unhealthyMeals) {
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
    if (completed) {
        exercise.markCompleted();
    }
}

int DailyRecord::dailyScore() const{
    return water.calculateScore() + nutrition.calculateScore()
        + exercise.calculateScore();
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
