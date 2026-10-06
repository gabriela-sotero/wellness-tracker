#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Database.h"
#include "HabitService.h"
#include "User.h"

// Coordinates text menus and input. It borrows Database, composes a
// HabitService, and stores the authenticated account for the current session.
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
    void logMeal();
    void logExercise();
    void logSleep();
    void showDailyStats();
    void showProfile();
    void showPeriodStats(
        const std::string& period,
        const std::vector<std::string>& dates
    );
    void showHistoryMenu();
    void deleteAccount();
    int currentLevel() const;
    void reportLevelUp(int previousLevel) const;
    void logOut();

    // Menus. Return false when the app should exit.
    bool loggedOutMenu();
    bool loggedInMenu();

public:
    // Uses the connection created by the caller for all interface operations.
    ConsoleUI(Database& database);

    // Runs the menu loop until the user chooses to exit.
    void run();
};
