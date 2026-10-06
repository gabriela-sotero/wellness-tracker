#pragma once

#include <string>
#include "ExerciseHabit.h"
#include "NutritionHabit.h"
#include "SleepHabit.h"
#include "WaterHabit.h"

// Aggregates one user's habits for one date. By composition, each daily record
// owns concrete habit objects that calculate their own progress and score.
class DailyRecord {
    private:
        WaterHabit water;
        NutritionHabit nutrition;
        ExerciseHabit exercise;
        SleepHabit sleep;
        std::string date;
        int userId;
    public:
        // Creates a record associated with a user, date, and water goal.
        DailyRecord(int userId, std::string date, int dailyGoalMl);

        // Normal logging operations used when a person records an activity.
        void logWater(int ml);
        void logMeal(bool healthy);
        // Restores persisted totals directly instead of replaying each action.
        void seedMeals(int healthyMeals, int unhealthyMeals);
        void logExercise();
        void seedExercise(bool completed);
        void logSleep(double hours);
        void seedSleep(double hours);
        // Aggregated score and read-only access to this day's habit state.
        int dailyScore() const;
        int nutritionScore() const;
        int exerciseScore() const;
        bool exerciseCompleted() const;
        double sleptHours() const;
        int sleepScore() const;
        int waterScore() const;
        int consumedWaterMl() const;
        int healthyMealCount() const;
        int unhealthyMealCount() const;
        // Identity fields let persistence save this aggregate back to its row.
        int getUserId() const;
        const std::string& getDate() const;

};
