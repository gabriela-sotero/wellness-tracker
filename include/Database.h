#pragma once

#include <optional>
#include <sqlite3.h>
#include <string>

#include "User.h"
#include "DailyRecord.h"
#include "Constants.h"

// Encapsulates the SQLite connection and exposes typed persistence operations,
// keeping SQL details outside the models and user interfaces.
class Database {
private:
    // Resource owned by this instance and closed by its destructor.
    sqlite3* db;

public:
    // Opens or creates the database at the supplied path.
    Database(const std::string& path);
    // Closes the SQLite connection owned by this instance.
    ~Database();

    // Creates tables and relationships without replacing existing records.
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

    // Inserts an account and returns its ID, or -1 if insertion fails.
    int insertUser(
        const std::string& username,
        const std::string& name,
        const std::string& password,
        std::optional<double> weightKg = std::nullopt,
        int waterGoalMl = Constants::DEFAULT_WATER_GOAL_ML
    );

    // Queries convert database rows into User model objects.
    std::optional<User> getUserById(int id);

    // Updates the account's optional weight and daily water goal.
    bool updateUserMetrics(int userId, std::optional<double> weightKg, int waterGoalMl);

    // Returns the first day with tracked activity for this account.
    std::optional<std::string> firstDailyRecordDate(int userId);

    // Returns the day the account was created, absent for accounts made
    // before the column existed.
    std::optional<std::string> accountCreatedAt(int userId);

    // Returns no value when the username does not belong to an account.
    std::optional<User> getUserByUsername(
        const std::string& username
    );

    // Returns the account only when the supplied password matches its stored hash.
    std::optional<User> authenticate(
        const std::string& username,
        const std::string& password
    );

    // Permanently removes the account and all of its habit records.
    bool deleteUser(int userId);
};
