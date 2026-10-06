#include "ConsoleUI.h"

#include <cctype>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "Constants.h"

// Returns no answer when the input stream closes.
std::optional<bool> ConsoleUI::askYesNo(const std::string& question) {
    // Keep asking until input is valid; nullopt propagates end-of-input to callers.
    while (true) {
        std::string answer;

        std::cout << question << " (y/n): ";

        if (!(std::cin >> answer)) {
            return std::nullopt;
        }

        for (char& character : answer) {
            character = std::tolower(character);
        }

        if (answer == "y" || answer == "yes") {
            return true;
        }

        if (answer == "n" || answer == "no") {
            return false;
        }

        std::cout << "Please enter yes or no.\n";
    }
}
// Handles user sign-up through the console interface.
void ConsoleUI::signUpUser() {
    // Collect and validate account fields here; Database owns persistence.
    std::string username;
    std::string name;
    std::string password;
    std::optional<double> weightKg;

    std::cout << "Username: ";
    if (!(std::cin >> username)) {
        return;
    }

    if (database.getUserByUsername(username).has_value()) {
        std::cout << "This username is already registered. Please log in.\n";
        return;
    }

    std::cout << "Name: ";
    std::getline(std::cin >> std::ws, name);

    std::cout << "Password: ";
    std::cin >> password;

    int waterGoalMl = Constants::DEFAULT_WATER_GOAL_ML;

    auto wantsWeight = askYesNo("Do you want to provide your weight?");
    if (!wantsWeight.has_value()) {
        return;
    }

    if (*wantsWeight) {
        double enteredWeightKg;

        while (true) {
            std::string input;
            std::cout << "Weight in kg: ";
            if (!(std::cin >> input)) {
                return;
            }

            std::istringstream value(input);
            if ((value >> enteredWeightKg) && value.eof() &&
                std::isfinite(enteredWeightKg) && enteredWeightKg > 0 &&
                enteredWeightKg <= std::numeric_limits<int>::max() /
                    static_cast<double>(Constants::WATER_ML_PER_KG)) {
                break;
            }
            std::cout << "Please enter a valid positive number for your weight.\n";
        }
        weightKg = enteredWeightKg;
        waterGoalMl = static_cast<int>(enteredWeightKg * Constants::WATER_ML_PER_KG);
        std::cout << "Your daily water goal is " << waterGoalMl << " ml.\n";
    } else {
        auto wantsCustomGoal = askYesNo("Do you want to personalize your water goal?");
        if (!wantsCustomGoal.has_value()) {
            return;
        }

        if (*wantsCustomGoal) {
            while (true) {
                std::string input;
                std::cout << "Daily water goal in ml: ";
                if (!(std::cin >> input)) {
                    return;
                }

                std::istringstream value(input);
                if ((value >> waterGoalMl) && value.eof() && waterGoalMl > 0) {
                    break;
                }
                std::cout << "Please enter a positive whole number in ml.\n";
            }
        } else {
            std::cout << "Your daily water goal will be "
                      << Constants::DEFAULT_WATER_GOAL_ML << " ml by default.\n";
        }
    }

    int userId = database.insertUser(username, name, password, weightKg, waterGoalMl);

    if (userId == -1) {
        std::cout << "Failed to create user.\n";
        return;
    }

    std::cout << "User created successfully.\n";
}

void ConsoleUI::deleteAccount() {
    // Require both explicit confirmation and password re-authentication before deletion.
    const auto confirmed = askYesNo(
        "Permanently delete your account and all its habit data?"
    );
    if (!confirmed.has_value() || !*confirmed) {
        return;
    }

    std::string password;
    std::cout << "Confirm your password: ";
    if (!(std::cin >> password)) {
        return;
    }
    if (!database.authenticate(currentUser->getUsername(), password).has_value()) {
        std::cout << "Incorrect password. Account was not deleted.\n";
        return;
    }

    if (!database.deleteUser(currentUser->getId())) {
        std::cout << "Could not delete the account. Please try again.\n";
        return;
    }
    currentUser = std::nullopt;
    std::cout << "Account and associated data deleted.\n";
}

// Authenticates a user and starts a session on success.
void ConsoleUI::logInUser() {
    // Store the returned User value only after Database verifies the credentials.
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
