#include "DailyRecord.h"

DailyRecord::DailyRecord(
    int userId,
    std::string date,
    int dailyGoalMl
):
    water(dailyGoalMl),
    nutrition(3),
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

int DailyRecord::dailyScore() const{
    return water.calculateScore() + nutrition.calculateScore();
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
