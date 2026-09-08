#include <cassert>
#include <iostream>
#include <string>

#include "WaterHabit.h"

static void test_water_habit_empty() {
    WaterHabit w(2000);

    assert(w.progress() == 0.0);
    assert(w.calculateScore() == 0);
    assert(w.name() == "Water Habit");
}

static void test_water_habit_progress_and_cap() {
    WaterHabit w(2000);

    w.drankWater(1000);
    assert(w.progress() == 0.50);
    assert(w.calculateScore() == 10);

    w.drankWater(5000);
    assert(w.progress() == 1.0);
    assert(w.calculateScore() == 20);
}

int main() {
    test_water_habit_empty();
    test_water_habit_progress_and_cap();

    std::cout << "All tests passed\n";
    return 0;
}
