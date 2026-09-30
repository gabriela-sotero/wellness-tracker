#pragma once

#include "Database.h"

struct NutritionSummary {
    int healthyMeals;
    int unhealthyMeals;
    int points;
};

struct ExerciseSummary {
    bool completed;
    int points;
};

class HabitService {
    private:
        Database& database;

    public:
        HabitService(Database& database);

        void logWaterHabit(int userId, int ml) const;
        void logMealHabit(int userId, bool healthy) const;
        void logExerciseHabit(int userId) const;

        int dailyScore(int userId, const std::string& date) const;
        int consumedWaterMl(int userId, const std::string& date) const;
        NutritionSummary nutritionSummary(int userId, const std::string& date) const;
        ExerciseSummary exerciseSummary(int userId, const std::string& date) const;
};
