#include <algorithm>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "Constants.h"
#include "DailyRecord.h"
#include "Database.h"
#include "DateUtils.h"

int main(int argc, char* argv[]) {
    // --reset is explicit because this utility replaces the demo account's data.
    const bool resetDemo = argc == 2 && std::string(argv[1]) == "--reset";
    if (argc > 2 || (argc == 2 && !resetDemo)) {
        std::cerr << "Usage: seed_demo_profile [--reset]\n";
        return 2;
    }

    Database database("data/wellness.db");
    database.createTables();

    constexpr const char* username = "demo";
    constexpr const char* password = "demo";

    const auto existingDemo = database.getUserByUsername(username);
    if (existingDemo.has_value()) {
        if (!resetDemo) {
            std::cout << "The demo account already exists; no data was changed. "
                         "Run with --reset to replace its demo data.\n";
            return 0;
        }
        if (!database.deleteUser(existingDemo->getId())) {
            std::cerr << "Could not reset the demo account.\n";
            return 1;
        }
    }

    const int userId = database.insertUser(
        username, "Badge Demo", password, std::nullopt,
        Constants::DEFAULT_WATER_GOAL_ML
    );
    if (userId <= 0) {
        std::cerr << "Could not create the demo account.\n";
        return 1;
    }

    const std::vector<std::string> dates = util::lastDays(365);
    // A fixed seed makes generated preview history repeatable across runs.
    std::mt19937 random(20261005);
    std::uniform_real_distribution<double> chance(0.0, 1.0);
    std::uniform_real_distribution<double> sleepVariation(-1.3, 1.5);
    std::uniform_int_distribution<int> waterExtra(0, 800);
    std::uniform_int_distribution<int> waterShortfall(0, 1200);
    std::uniform_int_distribution<int> healthyMealsOnGoodDay(3, 4);
    std::uniform_int_distribution<int> healthyMealsOnRoughDay(0, 2);
    std::uniform_int_distribution<int> unhealthyMeals(0, 2);

    // Gradually raise the chance of meeting goals while retaining daily variation.
    for (std::size_t index = 0; index < dates.size(); ++index) {
        const double yearProgress = dates.size() > 1
            ? static_cast<double>(index) / static_cast<double>(dates.size() - 1)
            : 1.0;
        const double dayQuality = std::uniform_real_distribution<double>(-0.15, 0.15)(random);

        // Start with low adherence, then build toward better habits while
        // preserving misses and rough days throughout the year.
        const double waterChance = std::clamp(0.18 + 0.50 * yearProgress + dayQuality, 0.0, 1.0);
        const double mealChance = std::clamp(0.12 + 0.48 * yearProgress + dayQuality, 0.0, 1.0);
        const double exerciseChance = std::clamp(0.10 + 0.50 * yearProgress + dayQuality, 0.0, 1.0);
        const bool waterGoalMet = chance(random) < waterChance;
        const bool mealsGoalMet = chance(random) < mealChance;
        const bool exerciseCompleted = chance(random) < exerciseChance;

        DailyRecord record(
            userId, dates[index], Constants::DEFAULT_WATER_GOAL_ML
        );

        const int waterMl = waterGoalMet
            ? Constants::DEFAULT_WATER_GOAL_ML + waterExtra(random)
            : Constants::DEFAULT_WATER_GOAL_ML - waterShortfall(random);
        record.logWater(waterMl);

        const int healthyCount = mealsGoalMet
            ? healthyMealsOnGoodDay(random)
            : healthyMealsOnRoughDay(random);
        record.seedMeals(healthyCount, unhealthyMeals(random));
        record.seedExercise(exerciseCompleted);

        const double sleepHours = std::clamp(
            5.8 + 1.8 * yearProgress + dayQuality * 3.0 + sleepVariation(random),
            4.5,
            10.0
        );
        record.logSleep(sleepHours);
        database.saveDailyRecord(record);
    }

    std::cout << "Created a badge preview profile with a year of varied habit records.\n"
              << "Sign in with username: " << username << "\n"
              << "Password: " << password << "\n"
              << "Open Profile to see the water, meals, and exercise badges.\n";
    return 0;
}
