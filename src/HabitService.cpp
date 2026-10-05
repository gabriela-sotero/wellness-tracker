#include "HabitService.h"
#include "Constants.h"
#include "DateUtils.h"

#include <iostream>
#include <string>
#include <vector>

HabitService::HabitService(Database& database)
    : database(database) {
}

void HabitService::logWaterHabit(int userId, int ml) const {
    std::string date = util::today();

    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );   
        record.logWater(ml);
        database.saveDailyRecord(record);
    } else {
        std::cerr << "logWaterHabit: user " << userId << " not found.\n";
        return;
    }
}

void HabitService::logMealHabit(int userId, bool healthy) const {
    std::string date = util::today();
    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );
        record.logMeal(healthy);
        database.saveDailyRecord(record);
    } else {
        std::cerr << "logMealHabit: user " << userId << " not found.\n";
    }
}

void HabitService::logExerciseHabit(int userId) const {
    std::string date = util::today();
    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );
        record.logExercise();
        database.saveDailyRecord(record);
    } else {
        std::cerr << "logExerciseHabit: user " << userId << " not found.\n";
    }
}

void HabitService::logSleepHabit(int userId, double hours) const {
    std::string date = util::today();
    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );
        record.logSleep(hours);
        database.saveDailyRecord(record);
    } else {
        std::cerr << "logSleepHabit: user " << userId << " not found.\n";
    }
}

int HabitService::dailyScore(int userId, const std::string& date) const {
    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl());
        return record.dailyScore();
    }

    return 0;
}

int HabitService::consumedWaterMl(int userId, const std::string& date) const {
    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl());
        return record.consumedWaterMl();
    }

    return 0;
}

int HabitService::waterScore(int userId, const std::string& date) const {
    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );
        return record.waterScore();
    }

    return 0;
}

NutritionSummary HabitService::nutritionSummary(
    int userId,
    const std::string& date
) const {
    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );
        return {
            record.healthyMealCount(),
            record.unhealthyMealCount(),
            record.nutritionScore()
        };
    }

    return {0, 0, 0};
}

ExerciseSummary HabitService::exerciseSummary(
    int userId,
    const std::string& date
) const {
    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );
        return {record.exerciseCompleted(), record.exerciseScore()};
    }

    return {false, 0};
}

SleepSummary HabitService::sleepSummary(
    int userId,
    const std::string& date
) const {
    if (auto user = database.getUserById(userId)) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );
        return {record.sleptHours(), record.sleepScore()};
    }

    return {0.0, 0};
}

// The longest run of met days anywhere in the history.
static int longestStreak(const std::vector<bool>& met) {
    int longest = 0;
    int run = 0;

    for (bool day : met) {
        run = day ? run + 1 : 0;
        if (run > longest) {
            longest = run;
        }
    }

    return longest;
}

int badgeDaysFor(int streak) {
    int days = 0;

    for (int tier = 0; tier < Constants::BADGE_TIER_COUNT; ++tier) {
        if (streak >= Constants::BADGE_TIERS[tier]) {
            days = Constants::BADGE_TIERS[tier];
        }
    }

    return days;
}

// Counts the days met in a row ending at the last entry. Today is still in
// progress, so an unmet goal today does not break a streak held yesterday.
static int currentStreak(const std::vector<bool>& met) {
    int streak = 0;
    auto day = met.rbegin();

    if (day != met.rend() && !*day) {
        ++day;
    }
    for (; day != met.rend() && *day; ++day) {
        ++streak;
    }

    return streak;
}

PeriodSummary HabitService::periodSummary(
    int userId,
    const std::vector<std::string>& dates
) const {
    PeriodSummary summary{};
    auto user = database.getUserById(userId);

    if (!user.has_value() || dates.empty()) {
        return summary;
    }

    summary.firstDate = dates.front();
    summary.lastDate = dates.back();
    summary.days = static_cast<int>(dates.size());
    summary.waterGoalMl = user->getWaterGoalMl() * summary.days;
    summary.sleepGoalHours = Constants::DEFAULT_SLEEP_GOAL_HOURS * summary.days;

    for (const std::string& date : dates) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );
        summary.consumedWaterMl += record.consumedWaterMl();
        summary.waterPoints += record.waterScore();
        summary.healthyMeals += record.healthyMealCount();
        summary.unhealthyMeals += record.unhealthyMealCount();
        summary.nutritionPoints += record.nutritionScore();
        summary.exerciseDays += record.exerciseCompleted() ? 1 : 0;
        summary.exercisePoints += record.exerciseScore();
        summary.sleepHours += record.sleptHours();
        summary.sleepPoints += record.sleepScore();
        summary.totalPoints += record.dailyScore();
    }

    return summary;
}

ProfileSummary HabitService::profileSummary(int userId) const {
    ProfileSummary summary{};
    auto user = database.getUserById(userId);
    auto firstDate = database.firstDailyRecordDate(userId);

    if (!user.has_value() || !firstDate.has_value()) {
        return summary;
    }

    std::vector<bool> waterGoalMet;
    std::vector<bool> healthyMealsGoalMet;
    std::vector<bool> exerciseGoalMet;
    std::vector<bool> sleepGoalMet;

    for (const std::string& date : util::datesBetween(*firstDate, util::today())) {
        DailyRecord record = database.loadOrCreateDailyRecord(
            userId, date, user->getWaterGoalMl()
        );
        summary.waterXp += record.waterScore();
        summary.nutritionXp += record.nutritionScore();
        summary.exerciseXp += record.exerciseScore();
        summary.sleepXp += record.sleepScore();

        waterGoalMet.push_back(record.consumedWaterMl() >= user->getWaterGoalMl());
        healthyMealsGoalMet.push_back(
            record.healthyMealCount() >= Constants::HEALTHY_MEALS_GOAL
        );
        exerciseGoalMet.push_back(record.exerciseCompleted());
        sleepGoalMet.push_back(record.sleptHours() >= Constants::DEFAULT_SLEEP_GOAL_HOURS);
    }

    summary.totalXp = summary.waterXp + summary.nutritionXp
        + summary.exerciseXp + summary.sleepXp;
    summary.waterStreak = currentStreak(waterGoalMet);
    summary.healthyMealsStreak = currentStreak(healthyMealsGoalMet);
    summary.exerciseStreak = currentStreak(exerciseGoalMet);
    summary.sleepStreak = currentStreak(sleepGoalMet);
    summary.waterBestStreak = longestStreak(waterGoalMet);
    summary.healthyMealsBestStreak = longestStreak(healthyMealsGoalMet);
    summary.exerciseBestStreak = longestStreak(exerciseGoalMet);
    summary.sleepBestStreak = longestStreak(sleepGoalMet);

    return summary;
}
