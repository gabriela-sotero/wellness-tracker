#pragma once

#include <string>

#include "Constants.h"
#include "Habit.h"

class SleepHabit : public Habit {
private:
    double goalHours;
    double sleptHours;

public:
    explicit SleepHabit(
        double goalHours = Constants::DEFAULT_SLEEP_GOAL_HOURS
    );

    int calculateScore() const override;
    double progress() const override;
    std::string name() const override;

    void logSleep(double hours);
    double getSleptHours() const;
};
