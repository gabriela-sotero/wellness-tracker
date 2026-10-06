#pragma once

#include <string>
#include "Habit.h"

// Tracks meal counts by type and applies the daily nutrition goal.
class NutritionHabit : public Habit {
private:
    int mealGoal;
    int healthyMeals;
    int unhealthyMeals;

public:
    // Sets how many healthy meals make up the daily goal.
    NutritionHabit(int mealGoal);

    int calculateScore() const override;
    double progress() const override;
    std::string name() const override;

    // Each action updates only the counter for its meal type.
    void logHealthyMeal();
    void logUnhealthyMeal();
    int healthyMealCount() const;
    int unhealthyMealCount() const;
};
