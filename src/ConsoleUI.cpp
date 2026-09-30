#include "ConsoleUI.h"

#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "Constants.h"
#include "DateUtils.h"

ConsoleUI::ConsoleUI(Database& database)
    : database(database),
      habitService(database),
      currentUser(std::nullopt) {
}

// Returns no answer when the input stream closes.
std::optional<bool> ConsoleUI::askYesNo(const std::string& question) {
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

// Shows the logged-in user's day: goal, intake, progress and XP.
void ConsoleUI::showDailyStats() {
    std::string date = util::today();
    int userId = currentUser->getId();

    int goalMl = currentUser->getWaterGoalMl();
    int consumedMl = habitService.consumedWaterMl(userId, date);
    int waterPoints = habitService.waterScore(userId, date);
    NutritionSummary meals = habitService.nutritionSummary(userId, date);
    ExerciseSummary exercise = habitService.exerciseSummary(userId, date);
    SleepSummary sleep = habitService.sleepSummary(userId, date);
    int score = habitService.dailyScore(userId, date);

    int percent = goalMl > 0 ? (consumedMl * 100) / goalMl : 0;

    std::cout << "\n--- My day (" << date << ") ---\n";
    std::cout << "Water intake: " << consumedMl << " / " << goalMl
              << " ml (" << percent << "%) (" << waterPoints << " XP)\n";
    std::cout << "Meals:        " << meals.healthyMeals << " healthy, "
              << meals.unhealthyMeals << " unhealthy (" << meals.points
              << " XP)\n";
    std::cout << "Exercise:     " << (exercise.completed ? "Completed" : "Not completed")
              << " (" << exercise.points << " XP)\n";
    int sleepPercent = static_cast<int>(
        (sleep.hours * 100.0) / Constants::DEFAULT_SLEEP_GOAL_HOURS
    );
    std::cout << "Sleep:        " << sleep.hours << " / "
              << Constants::DEFAULT_SLEEP_GOAL_HOURS << " hours ("
              << sleepPercent << "%) (" << sleep.points << " XP)\n";
    std::cout << "Daily XP: " << score << "\n\n";
}

void ConsoleUI::showProfile() {
    const int userId = currentUser->getId();
    const int waterGoalMl = currentUser->getWaterGoalMl();
    const std::string today = util::today();
    const auto firstDate = database.firstDailyRecordDate(userId);

    int waterXp = 0;
    int nutritionXp = 0;
    int exerciseXp = 0;
    int sleepXp = 0;
    std::vector<bool> waterGoalMet;
    std::vector<bool> healthyMealsGoalMet;
    std::vector<bool> exerciseGoalMet;
    std::vector<bool> sleepGoalMet;

    if (firstDate.has_value()) {
        std::tm date{};
        std::istringstream input(*firstDate);
        input >> std::get_time(&date, "%Y-%m-%d");
        if (!input.fail()) {
            date.tm_hour = 12; // Noon avoids daylight-saving transitions at midnight.
            std::mktime(&date);
            while (true) {
                char buffer[11];
                std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &date);
                const std::string currentDate(buffer);
                if (currentDate > today) {
                    break;
                }

                const int water = habitService.consumedWaterMl(userId, currentDate);
                const NutritionSummary meals = habitService.nutritionSummary(userId, currentDate);
                const ExerciseSummary exercise = habitService.exerciseSummary(userId, currentDate);
                const SleepSummary sleep = habitService.sleepSummary(userId, currentDate);
                waterXp += habitService.waterScore(userId, currentDate);
                nutritionXp += meals.points;
                exerciseXp += exercise.points;
                sleepXp += sleep.points;
                waterGoalMet.push_back(water >= waterGoalMl);
                healthyMealsGoalMet.push_back(meals.healthyMeals >= 3);
                exerciseGoalMet.push_back(exercise.completed);
                sleepGoalMet.push_back(sleep.hours > 8.0);

                date.tm_mday += 1;
                std::mktime(&date);
            }
        }
    }

    const auto currentStreak = [](const std::vector<bool>& met) {
        int streak = 0;
        auto day = met.rbegin();
        // Today is still in progress, so an unmet goal today does not break
        // a streak that was active through yesterday.
        if (day != met.rend() && !*day) {
            ++day;
        }
        for (; day != met.rend() && *day; ++day) {
            ++streak;
        }
        return streak;
    };

    std::cout << "\n--- Profile: " << currentUser->getName() << " ---\n";
    std::cout << "XP by habit\n";
    std::cout << "Water:     " << waterXp << " XP\n";
    std::cout << "Nutrition: " << nutritionXp << " XP\n";
    std::cout << "Exercise:  " << exerciseXp << " XP\n";
    std::cout << "Sleep:     " << sleepXp << " XP\n";
    std::cout << "Total:     " << waterXp + nutritionXp + exerciseXp + sleepXp << " XP\n";
    std::cout << "\nCurrent goal streaks (days)\n";
    std::cout << "Water goal met:      " << currentStreak(waterGoalMet) << "\n";
    std::cout << "3 healthy meals:     " << currentStreak(healthyMealsGoalMet) << "\n";
    std::cout << "Exercise completed:  " << currentStreak(exerciseGoalMet) << "\n";
    std::cout << "More than 8h sleep:  " << currentStreak(sleepGoalMet) << "\n\n";
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
        std::cout << "Please choose a valid option.\n";
    }

    return true;
}

bool ConsoleUI::loggedInMenu() {
    std::string option;

    std::cout << "\n1. Log action\n";
    std::cout << "2. View my day\n";
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
        showDailyStats();
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
