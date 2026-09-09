#pragma once
#include <string>
#include "Habit.h"

class WaterHabit : public Habit {
private:
    int goalMl;
    int consumedMl;
public:
    WaterHabit(int goalMl);

    int calculateScore() const override;
    double progress() const override;
    std::string name() const override;
    
    void drankWater(int ml);
    int getConsumedMl() const;
};