#pragma once

#include <string>
#include "Habit.h"

class NutritionHabit : public Habit {
private:
    int mealGoal;
    int healthyMeals;
    int unhealthyMeals;

public:
    NutritionHabit(int mealGoal);

    int calculateScore() const override;
    double progress() const override;
    std::string name() const override;

    void logHealthyMeal();
    void logUnhealthyMeal();
};