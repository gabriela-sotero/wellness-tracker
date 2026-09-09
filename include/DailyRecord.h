#pragma once

#include <string>
#include "WaterHabit.h"

class DailyRecord {
    private:
        WaterHabit water;
        std::string date;
        int userId;
    public:
        DailyRecord(int userId, std::string date, int dailyGoalMl);

        void logWater(int ml);
        int dailyScore() const;
        int consumedWaterMl() const;
        int getUserId() const;
        const std::string& getDate() const;

};