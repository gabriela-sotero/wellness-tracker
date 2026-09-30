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

namespace {
std::string formatDate(const std::tm& date) {
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &date);
    return buffer;
}

std::vector<std::string> datesForPeriod(int daysBack, bool calendarMonth, bool calendarYear) {
    std::time_t now = std::time(nullptr);
    std::tm today = *std::localtime(&now);
    std::tm first = today;
    if (calendarYear) {
        first.tm_mon = 0;
        first.tm_mday = 1;
    } else if (calendarMonth) {
        first.tm_mday = 1;
    } else {
        first.tm_mday -= daysBack;
    }
    first.tm_hour = 12;
    first.tm_min = 0;
    first.tm_sec = 0;
    std::mktime(&first);

    std::vector<std::string> dates;
    std::tm current = first;
    while (formatDate(current) <= formatDate(today)) {
        dates.push_back(formatDate(current));
        current.tm_mday += 1;
        std::mktime(&current);
    }
    return dates;
}
}

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

// Shows the logged-in user's day: goal, intake, progress and points.
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
              << " ml (" << percent << "%) (" << waterPoints << " points)\n";
    std::cout << "Meals:        " << meals.healthyMeals << " healthy, "
              << meals.unhealthyMeals << " unhealthy (" << meals.points
              << " points)\n";
    std::cout << "Exercise:     " << (exercise.completed ? "Completed" : "Not completed")
              << " (" << exercise.points << " points)\n";
    int sleepPercent = static_cast<int>(
        (sleep.hours * 100.0) / Constants::DEFAULT_SLEEP_GOAL_HOURS
    );
    std::cout << "Sleep:        " << sleep.hours << " / "
              << Constants::DEFAULT_SLEEP_GOAL_HOURS << " hours ("
              << sleepPercent << "%) (" << sleep.points << " points)\n";
    std::cout << "Daily points: " << score << "\n\n";
}

void ConsoleUI::showPeriodStats(
    const std::string& period,
    int daysBack,
    bool calendarMonth,
    bool calendarYear
) {
    const std::vector<std::string> dates = datesForPeriod(daysBack, calendarMonth, calendarYear);
    const int userId = currentUser->getId();
    const int goalMl = currentUser->getWaterGoalMl();
    int consumedMl = 0;
    int waterPoints = 0;
    int healthyMeals = 0;
    int unhealthyMeals = 0;
    int nutritionPoints = 0;
    int exerciseDays = 0;
    int exercisePoints = 0;
    double sleepHours = 0.0;
    int sleepPoints = 0;
    int dailyPoints = 0;

    for (const std::string& date : dates) {
        consumedMl += habitService.consumedWaterMl(userId, date);
        waterPoints += habitService.waterScore(userId, date);
        const NutritionSummary meals = habitService.nutritionSummary(userId, date);
        healthyMeals += meals.healthyMeals;
        unhealthyMeals += meals.unhealthyMeals;
        nutritionPoints += meals.points;
        const ExerciseSummary exercise = habitService.exerciseSummary(userId, date);
        exerciseDays += exercise.completed ? 1 : 0;
        exercisePoints += exercise.points;
        const SleepSummary sleep = habitService.sleepSummary(userId, date);
        sleepHours += sleep.hours;
        sleepPoints += sleep.points;
        dailyPoints += habitService.dailyScore(userId, date);
    }

    const int periodGoalMl = goalMl * static_cast<int>(dates.size());
    const int waterPercent = periodGoalMl > 0 ? consumedMl * 100 / periodGoalMl : 0;
    const double sleepGoalHours = Constants::DEFAULT_SLEEP_GOAL_HOURS * dates.size();
    const int sleepPercent = sleepGoalHours > 0
        ? static_cast<int>(sleepHours * 100.0 / sleepGoalHours)
        : 0;
    const std::streamsize oldPrecision = std::cout.precision();
    std::cout << "\n--- " << period << " (" << dates.front() << " to " << dates.back() << ") ---\n";
    std::cout << "Water intake: " << consumedMl << " / " << periodGoalMl
              << " ml (" << waterPercent << "%) (" << waterPoints << " points)\n";
    std::cout << "Meals:        " << healthyMeals << " healthy, " << unhealthyMeals
              << " unhealthy (" << nutritionPoints << " points)\n";
    std::cout << "Exercise:     " << exerciseDays << " days completed ("
              << exercisePoints << " points)\n";
    std::cout << "Sleep:        " << std::fixed << std::setprecision(1) << sleepHours
              << " / " << sleepGoalHours << " hours (" << sleepPercent << "%) ("
              << sleepPoints << " points)\n";
    std::cout << "Total points: " << dailyPoints << "\n\n";
    std::cout.unsetf(std::ios::floatfield);
    std::cout.precision(oldPrecision);
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
        showPeriodStats("Weekly", 6, false, false);
    } else if (option == "3") {
        showPeriodStats("Monthly", 0, true, false);
    } else if (option == "4") {
        showPeriodStats("Yearly", 0, false, true);
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
