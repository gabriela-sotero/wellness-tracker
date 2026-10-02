#pragma once

namespace Constants{
    constexpr int DEFAULT_WATER_GOAL_ML = 2000; // Used when the user gives no goal.
    constexpr int WATER_ML_PER_KG = 35;
    constexpr int WATER_MAX_SCORE = 500;
    constexpr int SLEEP_MAX_SCORE = 500;
    constexpr double DEFAULT_SLEEP_GOAL_HOURS = 8.0;
    constexpr int EXERCISE_MAX_SCORE = 300;
    constexpr int NUTRITION_MAX_SCORE = 300; // 3 per day
    constexpr int NUTRITION_UNHEALTHY_PENALTY = 50;
    constexpr int HEALTHY_MEALS_GOAL = 3; // Healthy meals needed to keep the streak.
}
