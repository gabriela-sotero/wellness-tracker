#include "NutritionHabit.h"
#include "Constants.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

NutritionHabit::NutritionHabit(int mealGoal)
    : mealGoal(mealGoal),
      healthyMeals(0),
      unhealthyMeals(0) {
    if (mealGoal <= 0) {
        throw std::invalid_argument(
            "Meal goal must be greater than zero."
        );
    }
}

void NutritionHabit::logHealthyMeal() {
    healthyMeals += 1;
}

void NutritionHabit::logUnhealthyMeal() {
    unhealthyMeals += 1;
}

double NutritionHabit::progress() const {
    double ratio = static_cast<double>(healthyMeals) / mealGoal;
    return std::min(ratio, 1.0);
}

int NutritionHabit::calculateScore() const {
    int earnedPoints = static_cast<int>(
        std::round(progress() * Constants::NUTRITION_MAX_SCORE)
    );

    int penalty =
        unhealthyMeals * Constants::NUTRITION_UNHEALTHY_PENALTY;

    return std::max(earnedPoints - penalty, 0);
}

std::string NutritionHabit::name() const {
    return "Nutrition Habit";
}