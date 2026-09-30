#pragma once

#include <string>
#include "ExerciseHabit.h"
#include "NutritionHabit.h"
#include "WaterHabit.h"

class DailyRecord {
    private:
        WaterHabit water;
        NutritionHabit nutrition;
        ExerciseHabit exercise;
        std::string date;
        int userId;
    public:
        DailyRecord(int userId, std::string date, int dailyGoalMl);

        void logWater(int ml);
        void logMeal(bool healthy);
        void seedMeals(int healthyMeals, int unhealthyMeals);
        void logExercise();
        void seedExercise(bool completed);
        int dailyScore() const;
        int nutritionScore() const;
        int exerciseScore() const;
        bool exerciseCompleted() const;
        int consumedWaterMl() const;
        int healthyMealCount() const;
        int unhealthyMealCount() const;
        int getUserId() const;
        const std::string& getDate() const;

};
