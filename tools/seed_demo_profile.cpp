#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "Constants.h"
#include "DailyRecord.h"
#include "Database.h"
#include "DateUtils.h"

int main() {
    Database database("data/wellness.db");
    database.createTables();

    constexpr const char* username = "badge_demo";
    constexpr const char* password = "demo1234";

    if (database.getUserByUsername(username).has_value()) {
        std::cout << "The badge_demo account already exists; no data was changed.\n";
        return 0;
    }

    const int userId = database.insertUser(
        username, "Badge Demo", password, std::nullopt,
        Constants::DEFAULT_WATER_GOAL_ML
    );
    if (userId <= 0) {
        std::cerr << "Could not create the badge_demo account.\n";
        return 1;
    }

    const std::vector<std::string> dates = util::lastDays(365);
    for (std::size_t index = 0; index < dates.size(); ++index) {
        const int daysAgo = static_cast<int>(dates.size() - index - 1);
        DailyRecord record(
            userId, dates[index], Constants::DEFAULT_WATER_GOAL_ML
        );

        // One full year of water, 90 days of healthy meals, and 30 days of
        // exercise produce distinct profile badge tiers.
        record.logWater(Constants::DEFAULT_WATER_GOAL_ML);
        record.seedMeals(daysAgo < 90 ? Constants::HEALTHY_MEALS_GOAL : 0, 0);
        record.seedExercise(daysAgo < 30);
        record.logSleep(7.0 + static_cast<double>(index % 5) * 0.5);
        database.saveDailyRecord(record);
    }

    std::cout << "Created a badge preview profile with 365 days of habit records.\n"
              << "Sign in with username: " << username << "\n"
              << "Password: " << password << "\n"
              << "Open Profile to see the water, meals, and exercise badges.\n";
    return 0;
}
