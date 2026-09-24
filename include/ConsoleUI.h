#pragma once

#include <optional>

#include "Database.h"
#include "HabitService.h"
#include "User.h"

class ConsoleUI {
private:
    Database& database;
    HabitService habitService;
    std::optional<User> currentUser;   // set while a user is logged in

    // Logged-out actions.
    void signUpUser();
    void loginUser();

    // Logged-in actions.
    void logWater();
    void showDailyStats();
    void logout();

    // Menus. Return false when the app should exit.
    bool loggedOutMenu();
    bool loggedInMenu();

public:
    ConsoleUI(Database& database);

    void run();
};
