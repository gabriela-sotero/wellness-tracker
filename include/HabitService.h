#pragma once

#include <string>
#include <vector>

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

struct SleepSummary {
    double hours;
    int points;
};

// Totals for a range of days, already added up for the interface to show.
struct PeriodSummary {
    std::string firstDate;
    std::string lastDate;
    int days;
    int consumedWaterMl;
    int waterGoalMl;        // Goal for the whole period, not for one day.
    int waterPoints;
    int healthyMeals;
    int unhealthyMeals;
    int nutritionPoints;
    int exerciseDays;
    int exercisePoints;
    double sleepHours;
    double sleepGoalHours;  // Goal for the whole period, not for one day.
    int sleepPoints;
    int totalPoints;
};

// Lifetime XP, the goal streaks running up to today, and the longest ones
// ever reached, which is what the badges are unlocked by.
struct ProfileSummary {
    int waterXp;
    int nutritionXp;
    int exerciseXp;
    int sleepXp;
    int totalXp;
    int waterStreak;
    int healthyMealsStreak;
    int exerciseStreak;
    int sleepStreak;
    int waterBestStreak;
    int healthyMealsBestStreak;
    int exerciseBestStreak;
    int sleepBestStreak;
};

// The highest badge tier a streak has reached, or 0 when none is unlocked yet.
int badgeDaysFor(int streak);

class HabitService {
    private:
        Database& database;

    public:
        HabitService(Database& database);

        void logWaterHabit(int userId, int ml) const;
        void logMealHabit(int userId, bool healthy) const;
        void logExerciseHabit(int userId) const;
        void logSleepHabit(int userId, double hours) const;

        int dailyScore(int userId, const std::string& date) const;
        int consumedWaterMl(int userId, const std::string& date) const;
        int waterScore(int userId, const std::string& date) const;
        NutritionSummary nutritionSummary(int userId, const std::string& date) const;
        ExerciseSummary exerciseSummary(int userId, const std::string& date) const;
        SleepSummary sleepSummary(int userId, const std::string& date) const;

        // Adds up every day in dates. Use util::lastDays, util::monthToDate
        // or util::yearToDate to build the range.
        PeriodSummary periodSummary(
            int userId,
            const std::vector<std::string>& dates
        ) const;

        // Covers every day from the user's first record up to today.
        ProfileSummary profileSummary(int userId) const;
};
