#include "ConsoleUI.h"

#include <iostream>
#include <string>

ConsoleUI::ConsoleUI(Database& database)
    : database(database) {
}

// Handles user registration through the console interface.
void ConsoleUI::registerUser() {
    std::string username;
    std::string name;
    char wantsWeight;

    std::cout << "Username: ";
    std::cin >> username;

    std::cout << "Name: ";
    std::cin >> name;

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
            weightKg
        );
    } else {
        userId = database.insertUser(
            username,
            name
        );
    }

    if (userId == -1) {
        std::cout << "Failed to create user.\n";
        return;
    }

    std::cout << "User created successfully.\n";
}

// Looks up an existing user by username.
void ConsoleUI::loginUser() {
    std::string username;

    std::cout << "Username: ";
    std::cin >> username;

    auto user = database.getUserByUsername(username);

    if (!user.has_value()) {
        std::cout << "User not found.\n";
        return;
    }

    std::cout << "Login successful.\n";
    std::cout << "Welcome, " << user->getName() << "!\n";
}

void ConsoleUI::run() {
    int option;

    std::cout << "1. Register\n";
    std::cout << "2. Login\n";
    std::cout << "Choose an option: ";
    std::cin >> option;

    if (option == 1) {
        registerUser();
    } else if (option == 2) {
        loginUser();
    } else {
        std::cout << "Invalid option.\n";
    }
}