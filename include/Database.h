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

    // Reads the user's record, restoring water and meal totals, or returns a
    // fresh zeroed record with the given water goal when no record exists.
    // Read-only: the row is only created on saveDailyRecord.
    DailyRecord loadOrCreateDailyRecord(
        int userId,
        const std::string& date,
        int waterGoalMl
    );

    // Persists the record: ensures the daily_records row exists and upserts
    // water and meal totals into their daily log tables.
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
