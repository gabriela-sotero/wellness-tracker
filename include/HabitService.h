#pragma once

#include <string>
#include <vector>

#include "Database.h"

// Summary value objects returned to the UI so it does not need to query storage.
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

struct LevelProgress {
    int level;
    int xpIntoLevel;
    int xpForNextLevel;
};

// Calculates the level and XP into that level from lifetime XP.
// Level n starts at Constants::XP_LEVEL_BASE * (n - 1)^2 total XP.
LevelProgress levelProgressForXp(int totalXp);

// Aggregated totals for a range of days, ready for the interface to display.
struct PeriodSummary {
    // Inclusive date boundaries and number of calendar days in this summary.
    std::string firstDate;
    std::string lastDate;
    int days;
    int waterGoalDays;
    int consumedWaterMl;
    int waterGoalMl;        // Goal for the whole period, not for one day.
    int healthyMealsGoalDays;
    int waterPoints;
    int healthyMeals;
    int unhealthyMeals;
    int nutritionPoints;
    int exerciseDays;
    int exercisePoints;
    int sleepGoalDays;
    double sleepHours;
    double sleepGoalHours;  // Goal for the whole period, not for one day.
    int sleepPoints;
    int totalPoints;
    int overallGoalsMet;
};

// Lifetime XP, current streaks, and the best streaks that unlock badges.
struct ProfileSummary {
    // XP is lifetime total; current streaks end today, best streaks cover history.
    int waterXp;
    int nutritionXp;
    int exerciseXp;
    int sleepXp;
    int totalXp;
    LevelProgress levelProgress;
    int waterStreak;
    int healthyMealsStreak;
    int exerciseStreak;
    int sleepStreak;
    int waterBestStreak;
    int healthyMealsBestStreak;
    int exerciseBestStreak;
    int sleepBestStreak;
};

// Returns the highest badge tier reached, or zero if no tier is unlocked.
int badgeDaysFor(int streak);

// Application service between Database and the interfaces. It coordinates
// habit logging, scoring, period summaries, and streak calculations without
// owning the SQLite connection.
class HabitService {
    private:
        Database& database;

    public:
        // Borrows a database connection that must outlive this service.
        HabitService(Database& database);

        // Logging methods build a daily model, apply one action, then persist it.
        void logWaterHabit(int userId, int ml) const;
        void logMealHabit(int userId, bool healthy) const;
        void logExerciseHabit(int userId) const;
        void logSleepHabit(int userId, double hours) const;

        // Read-only queries load a day and return scores or compact summaries.
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
