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

    // Reads the user's record, restoring water, meal and exercise data, or returns a
    // fresh zeroed record with the given water goal when no record exists.
    // Read-only: the row is only created on saveDailyRecord.
    DailyRecord loadOrCreateDailyRecord(
        int userId,
        const std::string& date,
        int waterGoalMl
    );

    // Persists the record: ensures the daily_records row exists and upserts
    // water, meal and exercise data into their daily log tables.
    void saveDailyRecord(const DailyRecord& record);

    int insertUser(
        const std::string& username,
        const std::string& name,
        const std::string& password,
        std::optional<double> weightKg = std::nullopt,
        int waterGoalMl = Constants::DEFAULT_WATER_GOAL_ML
    );

    std::optional<User> getUserById(int id);

    // Updates the account's optional weight and daily water goal.
    bool updateUserMetrics(int userId, std::optional<double> weightKg, int waterGoalMl);

    // Returns the first day with tracked activity for this account.
    std::optional<std::string> firstDailyRecordDate(int userId);

    // Returns the day the account was created, absent for accounts made
    // before the column existed.
    std::optional<std::string> accountCreatedAt(int userId);

    std::optional<User> getUserByUsername(
        const std::string& username
    );

    // Returns the user when the password matches the stored hash, else nullopt.
    std::optional<User> authenticate(
        const std::string& username,
        const std::string& password
    );

    // Permanently removes the account and all of its habit records.
    bool deleteUser(int userId);
};
