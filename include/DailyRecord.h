#pragma once

#include <string>
#include "NutritionHabit.h"
#include "WaterHabit.h"

class DailyRecord {
    private:
        WaterHabit water;
        NutritionHabit nutrition;
        std::string date;
        int userId;
    public:
        DailyRecord(int userId, std::string date, int dailyGoalMl);

        void logWater(int ml);
        void logMeal(bool healthy);
        void seedMeals(int healthyMeals, int unhealthyMeals);
        int dailyScore() const;
        int consumedWaterMl() const;
        int healthyMealCount() const;
        int unhealthyMealCount() const;
        int getUserId() const;
        const std::string& getDate() const;

};
