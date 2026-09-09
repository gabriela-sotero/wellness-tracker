#include <cassert>
#include <cmath>
#include <initializer_list>
#include <stdexcept>

#include "NutritionHabit.h"
#include "Constants.h"

static void test_nutrition_initial_state() {
    constexpr int goalMeals = 3;
    NutritionHabit n(goalMeals);

    assert(n.progress() == 0.0);
    assert(n.calculateScore() == 0);
    assert(n.name() == "Nutrition Habit");
}

static void test_nutrition_progress_and_cap() {
    constexpr int goalMeals = 3;
    NutritionHabit n(goalMeals);

    n.logHealthyMeal();

    // Uses a tolerance when comparing fractional values.
    assert(std::abs(n.progress() - 1.0 / 3.0) < 0.000001);

    // One of three meals earns 50 points when the maximum is 100.
    assert(n.calculateScore() == 50);

    n.logHealthyMeal();
    n.logHealthyMeal();

    assert(n.progress() == 1.0);
    assert(n.calculateScore() == Constants::NUTRITION_MAX_SCORE);

    // An extra healthy meal must not exceed the maximum.
    n.logHealthyMeal();

    assert(n.progress() == 1.0);
    assert(n.calculateScore() == Constants::NUTRITION_MAX_SCORE);
}

static void test_nutrition_penalty() {
    NutritionHabit n(3);

    n.logHealthyMeal();
    n.logHealthyMeal();
    n.logHealthyMeal();
    n.logUnhealthyMeal();

    assert(n.progress() == 1.0);
    assert(
        n.calculateScore() ==
        Constants::NUTRITION_MAX_SCORE -
        Constants::NUTRITION_UNHEALTHY_PENALTY
    );
}

// Checks that a penalty cannot make the score negative.
static void test_nutrition_score_floor() {
    NutritionHabit n(3);

    n.logUnhealthyMeal();

    assert(n.calculateScore() == 0);
}

// Checks that zero and negative goals are rejected.
static void test_nutrition_invalid_goal() {
    for (int goal : {0, -1}) {
        bool rejected = false;

        try {
            NutritionHabit n(goal);
        } catch (const std::invalid_argument&) {
            rejected = true;
        }

        assert(rejected);
    }
}

// Exposes one entry point for all nutrition tests.
void runNutritionTests() {
    test_nutrition_initial_state();
    test_nutrition_progress_and_cap();
    test_nutrition_penalty();
    test_nutrition_score_floor();
    test_nutrition_invalid_goal();
}