#include "ConsoleUI.h"

#include <ctime>
#include <iomanip>
#include <iostream>
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
              << " ml (" << waterPercent << "%) (" << waterPoints << " XP)\n";
    std::cout << "Meals:        " << healthyMeals << " healthy, " << unhealthyMeals
              << " unhealthy (" << nutritionPoints << " XP)\n";
    std::cout << "Exercise:     " << exerciseDays << " days completed ("
              << exercisePoints << " XP)\n";
    std::cout << "Sleep:        " << std::fixed << std::setprecision(1) << sleepHours
              << " / " << sleepGoalHours << " hours (" << sleepPercent << "%) ("
              << sleepPoints << " XP)\n";
    std::cout << "Total XP: " << dailyPoints << "\n\n";
    std::cout.unsetf(std::ios::floatfield);
    std::cout.precision(oldPrecision);
}

