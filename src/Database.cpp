#include "Database.h"

#include <iostream>

Database::Database(const std::string& path)
    : db(nullptr) {
    int result = sqlite3_open(path.c_str(), &db);

    if (result != SQLITE_OK) {
        std::cerr << "Failed to open database: "
                  << sqlite3_errmsg(db) << '\n';
    } else {
        std::cout << "Database opened successfully.\n";
    }
}

Database::~Database() {
    if (db != nullptr) {
        sqlite3_close(db);
    }
}

void Database::createTables() {
    const char* sql = R"(
        PRAGMA foreign_keys = ON;

        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            name TEXT NOT NULL,
            weight_kg REAL,
            water_goal_ml INTEGER NOT NULL DEFAULT 2000
        );

        CREATE TABLE IF NOT EXISTS daily_records (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            date TEXT NOT NULL,
            UNIQUE(user_id, date),
            FOREIGN KEY(user_id) REFERENCES users(id)
        );

        CREATE TABLE IF NOT EXISTS water_logs (
            daily_record_id INTEGER PRIMARY KEY,
            consumed_ml INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY(daily_record_id) REFERENCES daily_records(id)
        );
    )";

    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        db,
        sql,
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to create tables: "
                  << errorMessage << '\n';

        sqlite3_free(errorMessage);
        return;
    }

    std::cout << "Tables created successfully.\n";
}

int Database::insertUser(
    const std::string& username,
    const std::string& name,
    std::optional<double> weightKg,
    int waterGoalMl
) {
    const char* sql =
        "INSERT INTO users (username, name, weight_kg, water_goal_ml) "
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare insert statement: "
                  << sqlite3_errmsg(db) << '\n';
        return -1;
    }

    result = sqlite3_bind_text(
        statement,
        1,
        username.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to bind username: "
                  << sqlite3_errmsg(db) << '\n';

        sqlite3_finalize(statement);
        return -1;
    }

    result = sqlite3_bind_text(
        statement,
        2,
        name.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to bind user name: "
                  << sqlite3_errmsg(db) << '\n';

        sqlite3_finalize(statement);
        return -1;
    }

    if (weightKg.has_value()) {
        result = sqlite3_bind_double(
            statement,
            3,
            weightKg.value()
        );
    } else {
        result = sqlite3_bind_null(statement, 3);
    }

    if (result != SQLITE_OK) {
        std::cerr << "Failed to bind user weight: "
                  << sqlite3_errmsg(db) << '\n';

        sqlite3_finalize(statement);
        return -1;
    }

    result = sqlite3_bind_int(statement, 4, waterGoalMl);

    if (result != SQLITE_OK) {
        std::cerr << "Failed to bind water goal: "
                  << sqlite3_errmsg(db) << '\n';

        sqlite3_finalize(statement);
        return -1;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to insert user: "
                  << sqlite3_errmsg(db) << '\n';

        sqlite3_finalize(statement);
        return -1;
    }

    int userId =
        static_cast<int>(sqlite3_last_insert_rowid(db));

    sqlite3_finalize(statement);

    return userId;
}

std::optional<User> Database::getUserById(int id) {
    const char* sql =
        "SELECT id, username, name, weight_kg, water_goal_ml "
        "FROM users "
        "WHERE id = ?;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare user query: "
                  << sqlite3_errmsg(db) << '\n';

        return std::nullopt;
    }

    result = sqlite3_bind_int(statement, 1, id);

    if (result != SQLITE_OK) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    int userId = sqlite3_column_int(statement, 0);

    std::string username =
        reinterpret_cast<const char*>(
            sqlite3_column_text(statement, 1)
        );

    std::string name =
        reinterpret_cast<const char*>(
            sqlite3_column_text(statement, 2)
        );

    std::optional<double> weightKg = std::nullopt;

    if (sqlite3_column_type(statement, 3) != SQLITE_NULL) {
        weightKg = sqlite3_column_double(statement, 3);
    }

    int waterGoalMl = sqlite3_column_int(statement, 4);

    sqlite3_finalize(statement);

    if (weightKg.has_value()) {
        return User(
            userId,
            username,
            name,
            waterGoalMl,
            weightKg.value()
        );
    }

    return User(userId, username, name, waterGoalMl);
}

std::optional<User> Database::getUserByUsername(
    const std::string& username
) {
    const char* sql =
        "SELECT id, username, name, weight_kg, water_goal_ml "
        "FROM users "
        "WHERE username = ?;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare username query: "
                  << sqlite3_errmsg(db) << '\n';

        return std::nullopt;
    }

    result = sqlite3_bind_text(
        statement,
        1,
        username.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    if (result != SQLITE_OK) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_ROW) {
        sqlite3_finalize(statement);
        return std::nullopt;
    }

    int userId = sqlite3_column_int(statement, 0);

    std::string storedUsername =
        reinterpret_cast<const char*>(
            sqlite3_column_text(statement, 1)
        );

    std::string name =
        reinterpret_cast<const char*>(
            sqlite3_column_text(statement, 2)
        );

    std::optional<double> weightKg = std::nullopt;

    if (sqlite3_column_type(statement, 3) != SQLITE_NULL) {
        weightKg = sqlite3_column_double(statement, 3);
    }

    int waterGoalMl = sqlite3_column_int(statement, 4);

    sqlite3_finalize(statement);

    if (weightKg.has_value()) {
        return User(
            userId,
            storedUsername,
            name,
            waterGoalMl,
            weightKg.value()
        );
    }

    return User(userId, storedUsername, name, waterGoalMl);
}

DailyRecord Database::loadOrCreateDailyRecord(
    int userId,
    const std::string& date,
    int waterGoalMl
) {
    // Always start from a fresh, zeroed record with the user's goal.
    // If the day already has data, we seed the consumed water below.
    DailyRecord record(userId, date, waterGoalMl);

    const char* sql =
        "SELECT wl.consumed_ml "
        "FROM daily_records dr "
        "JOIN water_logs wl ON wl.daily_record_id = dr.id "
        "WHERE dr.user_id = ? AND dr.date = ?;";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare daily record query: "
                  << sqlite3_errmsg(db) << '\n';

        return record;
    }

    sqlite3_bind_int(statement, 1, userId);
    sqlite3_bind_text(
        statement,
        2,
        date.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW) {
        int consumedMl = sqlite3_column_int(statement, 0);
        record.logWater(consumedMl);
    }

    sqlite3_finalize(statement);

    return record;
}

void Database::saveDailyRecord(const DailyRecord& record) {
    // 1) Ensure the day row exists (no-op if it already does).
    const char* insertDay =
        "INSERT INTO daily_records (user_id, date) "
        "VALUES (?, ?) "
        "ON CONFLICT(user_id, date) DO NOTHING;";

    sqlite3_stmt* dayStatement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        insertDay,
        -1,
        &dayStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare daily record insert: "
                  << sqlite3_errmsg(db) << '\n';
        return;
    }

    sqlite3_bind_int(dayStatement, 1, record.getUserId());
    sqlite3_bind_text(
        dayStatement,
        2,
        record.getDate().c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    result = sqlite3_step(dayStatement);
    sqlite3_finalize(dayStatement);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to insert daily record: "
                  << sqlite3_errmsg(db) << '\n';
        return;
    }

    // 2) Resolve the daily_record id for this (user, date).
    const char* selectId =
        "SELECT id FROM daily_records "
        "WHERE user_id = ? AND date = ?;";

    sqlite3_stmt* idStatement = nullptr;

    result = sqlite3_prepare_v2(
        db,
        selectId,
        -1,
        &idStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare daily record id query: "
                  << sqlite3_errmsg(db) << '\n';
        return;
    }

    sqlite3_bind_int(idStatement, 1, record.getUserId());
    sqlite3_bind_text(
        idStatement,
        2,
        record.getDate().c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    if (sqlite3_step(idStatement) != SQLITE_ROW) {
        std::cerr << "Failed to resolve daily record id: "
                  << sqlite3_errmsg(db) << '\n';

        sqlite3_finalize(idStatement);
        return;
    }

    int dailyRecordId = sqlite3_column_int(idStatement, 0);
    sqlite3_finalize(idStatement);

    // 3) Upsert the water total for the day.
    const char* upsertWater =
        "INSERT INTO water_logs (daily_record_id, consumed_ml) "
        "VALUES (?, ?) "
        "ON CONFLICT(daily_record_id) "
        "DO UPDATE SET consumed_ml = excluded.consumed_ml;";

    sqlite3_stmt* waterStatement = nullptr;

    result = sqlite3_prepare_v2(
        db,
        upsertWater,
        -1,
        &waterStatement,
        nullptr
    );

    if (result != SQLITE_OK) {
        std::cerr << "Failed to prepare water log upsert: "
                  << sqlite3_errmsg(db) << '\n';
        return;
    }

    sqlite3_bind_int(waterStatement, 1, dailyRecordId);
    sqlite3_bind_int(waterStatement, 2, record.consumedWaterMl());

    result = sqlite3_step(waterStatement);
    sqlite3_finalize(waterStatement);

    if (result != SQLITE_DONE) {
        std::cerr << "Failed to upsert water log: "
                  << sqlite3_errmsg(db) << '\n';
    }
}