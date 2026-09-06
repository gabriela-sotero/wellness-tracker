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
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            name TEXT NOT NULL,
            weight_kg REAL
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
    std::optional<double> weightKg
) {
    const char* sql =
        "INSERT INTO users (username, name, weight_kg) "
        "VALUES (?, ?, ?);";

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
        "SELECT id, username, name, weight_kg "
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

    sqlite3_finalize(statement);

    if (weightKg.has_value()) {
        return User(
            userId,
            username,
            name,
            weightKg.value()
        );
    }

    return User(userId, username, name);
}

std::optional<User> Database::getUserByUsername(
    const std::string& username
) {
    const char* sql =
        "SELECT id, username, name, weight_kg "
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

    sqlite3_finalize(statement);

    if (weightKg.has_value()) {
        return User(
            userId,
            storedUsername,
            name,
            weightKg.value()
        );
    }

    return User(userId, storedUsername, name);
}