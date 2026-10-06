#pragma once

#include <string>

#include "Constants.h"
#include "Habit.h"

// Compares the total hours slept with a configurable goal.
class SleepHabit : public Habit {
private:
    double goalHours;
    double sleptHours;

public:
    // Uses the default goal when no custom goal is supplied.
    explicit SleepHabit(
        double goalHours = Constants::DEFAULT_SLEEP_GOAL_HOURS
    );

    int calculateScore() const override;
    double progress() const override;
    std::string name() const override;

    // Records the number of hours slept for the day.
    void logSleep(double hours);
    double getSleptHours() const;
};
