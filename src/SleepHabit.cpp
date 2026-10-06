#include "SleepHabit.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

SleepHabit::SleepHabit(double goalHours)
    : goalHours(goalHours),
      sleptHours(0.0) {
    if (!std::isfinite(goalHours) || goalHours <= 0.0) {
        throw std::invalid_argument("Sleep goal must be a positive number of hours.");
    }
}

int SleepHabit::calculateScore() const {
    // Convert capped goal progress into the configured maximum sleep score.
    return static_cast<int>(std::round(progress() * Constants::SLEEP_MAX_SCORE));
}

double SleepHabit::progress() const {
    // Sleeping beyond the goal does not earn more than full progress.
    return std::min(sleptHours / goalHours, 1.0);
}

std::string SleepHabit::name() const {
    return "Sleep Habit";
}

void SleepHabit::logSleep(double hours) {
    if (!std::isfinite(hours) || hours < 0.0) {
        throw std::invalid_argument("Sleep duration must be a non-negative number of hours.");
    }
    sleptHours += hours;
}

double SleepHabit::getSleptHours() const {
    return sleptHours;
}
