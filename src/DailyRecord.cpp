#include "DailyRecord.h"

DailyRecord::DailyRecord(
    int userId,
    std::string date,
    int dailyGoalMl
):
    userId(userId),
    date(date),
    water(dailyGoalMl){
}

void DailyRecord::logWater(int ml) {
    water.drankWater(ml);
    return;
}

int DailyRecord::dailyScore() const{
    return water.calculateScore();
}

int DailyRecord::consumedWaterMl() const {
    return water.getConsumedMl();
}

int DailyRecord::getUserId() const {
    return userId;
}

const std::string& DailyRecord::getDate() const {
    return date;
}