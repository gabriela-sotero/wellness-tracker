#include "WaterHabit.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>
#include <cassert>

WaterHabit::WaterHabit(int goalMl):   
    goalMl(goalMl),
    consumedMl(0) { 
}

int WaterHabit::calculateScore() const { 
    return std::round(WaterHabit::progress() * Constants::WATER_MAX_SCORE);
}

double WaterHabit::progress() const { 
    double ratio = double(consumedMl) / goalMl;
    return std::min(ratio, 1.0);
}

std::string WaterHabit::name() const { 
    return "Water Habit";
}

void WaterHabit::drankWater(int ml) {
    // UI must ensure no negative input is accepted.
    // If ml is negative here, we did something wrong code-wise.
    assert(ml >= 0 && "drankWater ml is negative");
    consumedMl += ml;
}