#pragma once

#include <optional>
#include <string>

#include "Database.h"
#include "HabitService.h"
#include "User.h"

class ConsoleUI {
private:
    Database& database;
    HabitService habitService;
    std::optional<User> currentUser;   // set while a user is logged in

    std::optional<bool> askYesNo(const std::string& question);

    // Logged-out actions.
    void signUpUser();
    void logInUser();

    // Logged-in actions.
    void logWater();
    void showDailyStats();
    void logOut();

    // Menus. Return false when the app should exit.
    bool loggedOutMenu();
    bool loggedInMenu();

public:
    ConsoleUI(Database& database);

    void run();
};
