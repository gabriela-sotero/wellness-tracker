#include "ConsoleUI.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

// Logs water intake for the logged-in user.
void ConsoleUI::logWater() {
    int ml;

    while (true) {
        std::string input;
        std::cout << "How much water did you drink (ml)? ";
        if (!(std::cin >> input)) {
            return;
        }

        std::istringstream value(input);
        if ((value >> ml) && value.eof() && ml > 0) {
            break;
        }
        std::cout << "Please enter a positive whole number in ml.\n";
    }

    habitService.logWaterHabit(currentUser->getId(), ml);
    std::cout << "Logged " << ml << " ml of water.\n";
}

// Logs one meal and records whether it was healthy.
void ConsoleUI::logMeal() {
    auto healthy = askYesNo("Was this a healthy meal?");
    if (!healthy.has_value()) {
        return;
    }

    habitService.logMealHabit(currentUser->getId(), *healthy);
    std::cout << (*healthy ? "Logged a healthy meal.\n" : "Logged an unhealthy meal.\n");
}

// Marks today's exercise as completed.
void ConsoleUI::logExercise() {
    habitService.logExerciseHabit(currentUser->getId());
    std::cout << "Exercise marked as completed.\n";
}

// Logs the number of hours slept today.
void ConsoleUI::logSleep() {
    double hours = 0.0;
    while (true) {
        std::string input;
        std::cout << "How many hours did you sleep? ";
        if (!(std::cin >> input)) {
            return;
        }

        std::istringstream value(input);
        if ((value >> hours) && value.eof() &&
            std::isfinite(hours) && hours > 0.0) {
            break;
        }
        std::cout << "Please enter a positive number of hours.\n";
    }

    habitService.logSleepHabit(currentUser->getId(), hours);
    std::cout << "Logged " << hours << " hours of sleep.\n";
}

