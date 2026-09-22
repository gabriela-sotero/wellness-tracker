#include "ConsoleUI.h"

#include <iostream>
#include <string>

#include "Constants.h"
#include "DateUtils.h"

ConsoleUI::ConsoleUI(Database& database)
    : database(database),
      habitService(database),
      currentUser(std::nullopt) {
}

// Handles user registration through the console interface.
void ConsoleUI::registerUser() {
    std::string username;
    std::string name;
    std::string password;
    char wantsWeight;

    std::cout << "Username: ";
    std::cin >> username;

    std::cout << "Name: ";
    std::cin >> name;

    std::cout << "Password: ";
    std::cin >> password;

    int waterGoalMl;
    std::cout << "Daily water goal in ml (0 for default "
              << Constants::DEFAULT_WATER_GOAL_ML << "): ";
    std::cin >> waterGoalMl;

    if (waterGoalMl <= 0) {
        waterGoalMl = Constants::DEFAULT_WATER_GOAL_ML;
    }

    std::cout << "Do you want to provide your weight? (y/n): ";
    std::cin >> wantsWeight;

    int userId;

    if (wantsWeight == 'y' || wantsWeight == 'Y') {
        double weightKg;

        std::cout << "Weight in kg: ";
        std::cin >> weightKg;

        userId = database.insertUser(
            username,
            name,
            password,
            weightKg,
            waterGoalMl
        );
    } else {
        userId = database.insertUser(
            username,
            name,
            password,
            std::nullopt,
            waterGoalMl
        );
    }

    if (userId == -1) {
        std::cout << "Failed to create user.\n";
        return;
    }

    std::cout << "User created successfully.\n";
}

// Authenticates a user and starts a session on success.
void ConsoleUI::loginUser() {
    std::string username;
    std::string password;

    std::cout << "Username: ";
    std::cin >> username;

    std::cout << "Password: ";
    std::cin >> password;

    auto user = database.authenticate(username, password);

    if (!user.has_value()) {
        std::cout << "Invalid username or password.\n";
        return;
    }

    currentUser = user;

    std::cout << "Login successful.\n";
    std::cout << "Welcome, " << currentUser->getName() << "!\n";
}

// Logs water intake for the logged-in user.
void ConsoleUI::logWater() {
    int ml;

    std::cout << "How much water did you drink (ml)? ";
    std::cin >> ml;

    if (ml <= 0) {
        std::cout << "Please enter a positive amount.\n";
        return;
    }

    habitService.logWaterHabit(currentUser->getId(), ml);
    std::cout << "Logged " << ml << " ml of water.\n";
}

// Shows the logged-in user's day: goal, intake, progress and points.
void ConsoleUI::showDailyStats() {
    std::string date = util::today();
    int userId = currentUser->getId();

    int goalMl = currentUser->getWaterGoalMl();
    int consumedMl = habitService.consumedWaterMl(userId, date);
    int score = habitService.dailyScore(userId, date);

    int percent = goalMl > 0 ? (consumedMl * 100) / goalMl : 0;

    std::cout << "\n--- My day (" << date << ") ---\n";
    std::cout << "Hello, " << currentUser->getName() << "!\n";
    std::cout << "Water goal:   " << goalMl << " ml\n";
    std::cout << "Consumed:     " << consumedMl << " ml (" << percent << "%)\n";
    std::cout << "Daily points: " << score << "\n\n";
}

void ConsoleUI::logout() {
    currentUser = std::nullopt;
    std::cout << "Logged out.\n";
}

bool ConsoleUI::loggedOutMenu() {
    int option;

    std::cout << "\n1. Register\n";
    std::cout << "2. Login\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";

    if (!(std::cin >> option)) {
        return false;
    }

    if (option == 1) {
        registerUser();
    } else if (option == 2) {
        loginUser();
    } else if (option == 0) {
        std::cout << "Goodbye!\n";
        return false;
    } else {
        std::cout << "Invalid option.\n";
    }

    return true;
}

bool ConsoleUI::loggedInMenu() {
    int option;

    std::cout << "\n1. Log water\n";
    std::cout << "2. View my day\n";
    std::cout << "0. Logout\n";
    std::cout << "Choose an option: ";

    if (!(std::cin >> option)) {
        return false;
    }

    if (option == 1) {
        logWater();
    } else if (option == 2) {
        showDailyStats();
    } else if (option == 0) {
        logout();
    } else {
        std::cout << "Invalid option.\n";
    }

    return true;
}

void ConsoleUI::run() {
    bool running = true;

    while (running) {
        if (currentUser.has_value()) {
            running = loggedInMenu();
        } else {
            running = loggedOutMenu();
        }
    }
}
