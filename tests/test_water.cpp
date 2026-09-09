#include <cassert>

#include "WaterHabit.h"
#include "Constants.h"

// Checks that a new habit starts with no progress or points.
static void test_water_initial_state() {
    WaterHabit w(2000);

    assert(w.progress() == 0.0);
    assert(w.calculateScore() == 0);
    assert(w.name() == "Water Habit");
}

// Checks partial progress and caps progress and points at the goal.
static void test_water_progress_and_cap() {
    constexpr int goalMl = 2000;
    WaterHabit w(goalMl);

    // Half of the goal earns half of the maximum score.
    w.drankWater(goalMl / 2);

    assert(w.progress() == 0.50);
    assert(w.calculateScore() == Constants::WATER_MAX_SCORE / 2);

    // Exceeding the goal must not exceed the maximum score.
    w.drankWater(5000);

    assert(w.progress() == 1.0);
    assert(w.calculateScore() == Constants::WATER_MAX_SCORE);
}

// Exposes one entry point for all water tests.
void runWaterTests() {
    test_water_initial_state();
    test_water_progress_and_cap();
}