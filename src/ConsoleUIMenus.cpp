#include "ConsoleUI.h"

#include <iostream>
#include <string>

#include "DateUtils.h"

ConsoleUI::ConsoleUI(Database& database)
    : database(database),
      habitService(database),
      currentUser(std::nullopt) {
}

void ConsoleUI::logOut() {
    currentUser = std::nullopt;
    std::cout << "Logged out.\n";
}

void ConsoleUI::showHistoryMenu() {
    std::string option;
    std::cout << "\n--- View progress ---\n";
    std::cout << "1. Daily\n";
    std::cout << "2. Weekly\n";
    std::cout << "3. Monthly\n";
    std::cout << "4. Yearly\n";
    std::cout << "0. Back\n";
    std::cout << "Choose a period: ";

    if (!(std::cin >> option)) {
        return;
    }

    if (option == "1") {
        showDailyStats();
    } else if (option == "2") {
        showPeriodStats("Weekly", util::lastDays(7));
    } else if (option == "3") {
        showPeriodStats("Monthly", util::monthToDate());
    } else if (option == "4") {
        showPeriodStats("Yearly", util::yearToDate());
    } else if (option != "0") {
        std::cout << "Invalid option. Please choose a valid option.\n";
    }
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
        std::cout << "Please choose a valid option.\n";
    }

    return true;
}

bool ConsoleUI::loggedInMenu() {
    std::string option;

    std::cout << "\n1. Log action\n";
    std::cout << "2. View progress\n";
    std::cout << "3. View profile\n";
    std::cout << "0. Log out\n";
    std::cout << "Choose an option: ";

    if (!(std::cin >> option)) {
        return false;
    }

    if (option == "1") {
        std::string action;
        std::cout << "\n1. Water\n";
        std::cout << "2. Meal\n";
        std::cout << "3. Exercise\n";
        std::cout << "4. Sleep\n";
        std::cout << "0. Back\n";
        std::cout << "Choose an action: ";

        if (!(std::cin >> action)) {
            return false;
        }

        if (action == "1") {
            logWater();
        } else if (action == "2") {
            logMeal();
        } else if (action == "3") {
            logExercise();
        } else if (action == "4") {
            logSleep();
        } else if (action != "0") {
            std::cout << "Invalid action. Please choose a valid option.\n";
        }
    } else if (option == "2") {
        showHistoryMenu();
    } else if (option == "3") {
        showProfile();
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
