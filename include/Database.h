#pragma once

#include <optional>
#include <sqlite3.h>
#include <string>

#include "User.h"
#include "DailyRecord.h"
#include "Constants.h"

class Database {
private:
    sqlite3* db;

public:
    Database(const std::string& path);
    ~Database();

    void createTables();

    // Reads today's record for the user (seeding the consumed water) or,
    // if none exists yet, returns a fresh zeroed record with the given goal.
    // Read-only: the row is only created on saveDailyRecord.
    DailyRecord loadOrCreateDailyRecord(
        int userId,
        const std::string& date,
        int waterGoalMl
    );

    // Persists the record: ensures the daily_records row exists and upserts
    // the water total into water_logs.
    void saveDailyRecord(const DailyRecord& record);

    int insertUser(
        const std::string& username,
        const std::string& name,
        const std::string& password,
        std::optional<double> weightKg = std::nullopt,
        int waterGoalMl = Constants::DEFAULT_WATER_GOAL_ML
    );

    std::optional<User> getUserById(int id);

    std::optional<User> getUserByUsername(
        const std::string& username
    );

    // Returns the user when the password matches the stored hash, else nullopt.
    std::optional<User> authenticate(
        const std::string& username,
        const std::string& password
    );
};