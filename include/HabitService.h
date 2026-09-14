#pragma once

#include "Database.h"

class HabitService {
    private:
        Database& database;

    public:
        HabitService(Database& database);

        void logWaterHabit(int userId, int ml) const;
};