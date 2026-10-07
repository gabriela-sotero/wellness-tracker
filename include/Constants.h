#pragma once

namespace Constants{
    // Shared defaults, goal values, scoring caps, penalties, and XP settings.
    constexpr int DEFAULT_WATER_GOAL_ML = 2000; // Used when the user gives no goal.
    constexpr int WATER_ML_PER_KG = 35;
    constexpr int WATER_MAX_SCORE = 300;
    constexpr int SLEEP_MAX_SCORE = 300;
    constexpr double DEFAULT_SLEEP_GOAL_HOURS = 8.0;
    constexpr int EXERCISE_MAX_SCORE = 300;
    constexpr int HEALTHY_MEALS_GOAL = 3; // Healthy meals needed to keep the streak.
    constexpr int NUTRITION_MAX_SCORE = 300;
    // Half the XP for one healthy meal (300 / 3 / 2).
    constexpr int NUTRITION_UNHEALTHY_PENALTY =
        (NUTRITION_MAX_SCORE + 2 * HEALTHY_MEALS_GOAL - 1)
        / (2 * HEALTHY_MEALS_GOAL);
    constexpr int XP_LEVEL_BASE = 35; // Total XP thresholds scale with this value.
    // Badge thresholds in days, ordered from the smallest tier to the largest.
    constexpr int BADGE_TIERS[] = {
        3, 7, 15, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 365
    };
    constexpr int BADGE_TIER_COUNT = 15;
}
