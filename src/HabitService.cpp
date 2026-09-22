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
