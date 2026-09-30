#include "HabitService.h"
#include "DateUtils.h"

#include <iostream>
#include <string>

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
