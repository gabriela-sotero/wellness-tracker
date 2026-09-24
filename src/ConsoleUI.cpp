#include "ConsoleUI.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "Constants.h"
#include "DateUtils.h"

ConsoleUI::ConsoleUI(Database& database)
    : database(database),
      habitService(database),
      currentUser(std::nullopt) {
}

// Handles user sign-up through the console interface.
void ConsoleUI::signUpUser() {
    std::string username;
    std::string name;
    std::string password;
    std::string wantsWeight;

    std::cout << "Username: ";
    std::cin >> username;

    std::cout << "Name: ";
    std::cin >> name;

    std::cout << "Password: ";
    std::cin >> password;

    int waterGoalMl = Constants::DEFAULT_WATER_GOAL_ML;

    while (true) {
        std::cout << "Do you want to provide your weight? (y/n): ";
        if (!(std::cin >> wantsWeight)) {
            return;
        }
        if (wantsWeight == "y" || wantsWeight == "Y" ||
            wantsWeight == "n" || wantsWeight == "N") {
            break;
        }
        std::cout << "Please enter y or n.\n";
    }

    int userId;

    if (wantsWeight == "y" || wantsWeight == "Y") {
        double weightKg;

        while (true) {
            std::string input;
            std::cout << "Weight in kg: ";
            if (!(std::cin >> input)) {
                return;
            }

            std::istringstream value(input);
            if ((value >> weightKg) && value.eof() &&
                std::isfinite(weightKg) && weightKg > 0 &&
                weightKg <= std::numeric_limits<int>::max() / 35.0) {
                break;
            }
            std::cout << "Please enter a valid positive number for your weight.\n";
        }
        waterGoalMl = static_cast<int>(weightKg * 35);
        std::cout << "Your daily water goal is " << waterGoalMl << " ml.\n";

        userId = database.insertUser(
            username,
            name,
            password,
            weightKg,
            waterGoalMl
        );
    } else {
        if (wantsWeight == "n" || wantsWeight == "N") {
            std::string wantsCustomGoal;
            while (true) {
                std::cout << "Do you want to personalize your water goal? (y/n): ";
                if (!(std::cin >> wantsCustomGoal)) {
                    return;
                }
                if (wantsCustomGoal == "y" || wantsCustomGoal == "Y" ||
                    wantsCustomGoal == "n" || wantsCustomGoal == "N") {
                    break;
                }
                std::cout << "Please enter y or n.\n";
            }

            if (wantsCustomGoal == "y" || wantsCustomGoal == "Y") {
                std::cout << "Daily water goal in ml: ";
                std::cin >> waterGoalMl;

                if (waterGoalMl <= 0) {
                    waterGoalMl = Constants::DEFAULT_WATER_GOAL_ML;
                }
            } else if (wantsCustomGoal == "n" || wantsCustomGoal == "N") {
                std::cout << "Your daily water goal will be "
                          << Constants::DEFAULT_WATER_GOAL_ML << " ml by default.\n";
            }
        }

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
void ConsoleUI::logInUser() {
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

    std::cout << "Log in successful.\n";
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

void ConsoleUI::logOut() {
    currentUser = std::nullopt;
    std::cout << "Logged out.\n";
}

bool ConsoleUI::loggedOutMenu() {
    std::string option;

    std::cout << "\n1. Sign up\n";
    std::cout << "2. Log in\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";

    if (!(std::cin >> option)) {
        return false;
    }

    if (option == "1") {
        signUpUser();
    } else if (option == "2") {
        logInUser();
    } else if (option == "0") {
        std::cout << "Goodbye!\n";
        return false;
    } else {
        std::cout << "Invalid option. Please choose a valid option.\n";
    }

    return true;
}

bool ConsoleUI::loggedInMenu() {
    std::string option;

    std::cout << "\n1. Log water\n";
    std::cout << "2. View my day\n";
    std::cout << "0. Log out\n";
    std::cout << "Choose an option: ";

    if (!(std::cin >> option)) {
        return false;
    }

    if (option == "1") {
        logWater();
    } else if (option == "2") {
        showDailyStats();
    } else if (option == "0") {
        logOut();
    } else {
        std::cout << "Invalid option. Please choose a valid option.\n";
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
