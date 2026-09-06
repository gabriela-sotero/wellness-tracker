#pragma once

#include <optional>
#include <sqlite3.h>
#include <string>

#include "User.h"

class Database {
private:
    sqlite3* db;

public:
    Database(const std::string& path);
    ~Database();

    void createTables();

    int insertUser(
        const std::string& username,
        const std::string& name,
        std::optional<double> weightKg = std::nullopt
    );

    std::optional<User> getUserById(int id);

    std::optional<User> getUserByUsername(
        const std::string& username
    );
};