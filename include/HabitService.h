#pragma once

#include "Database.h"

class HabitService {
    private:
        Database& database;

    public:
        HabitService(Database& database);

        void logWaterHabit(int userId, int ml) const;

        int dailyScore(int userId, const std::string& date) const;
        int consumedWaterMl(int userId, const std::string& date) const;
};