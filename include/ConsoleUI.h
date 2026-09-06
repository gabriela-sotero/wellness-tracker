#pragma once

#include "Database.h"

class ConsoleUI {
private:
    Database& database;

    void registerUser();
    void loginUser();

public:
    ConsoleUI(Database& database);

    void run();
};