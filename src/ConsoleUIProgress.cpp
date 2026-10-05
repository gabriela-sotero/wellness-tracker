#include "ConsoleUI.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "DateUtils.h"

// Shows the logged-in user's day: goal, intake, progress and XP.
void ConsoleUI::showDailyStats() {
    const std::string date = util::today();
    const PeriodSummary day = habitService.periodSummary(currentUser->getId(), {date});

    const int waterPercent = day.waterGoalMl > 0
        ? (day.consumedWaterMl * 100) / day.waterGoalMl
        : 0;
    const int sleepPercent = day.sleepGoalHours > 0
        ? static_cast<int>(day.sleepHours * 100.0 / day.sleepGoalHours)
        : 0;

    std::cout << "\n--- My day (" << date << ") ---\n";
    std::cout << "Water intake: " << day.consumedWaterMl << " / " << day.waterGoalMl
              << " ml (" << waterPercent << "%) (" << day.waterPoints << " XP)\n";
    std::cout << "Meals:        " << day.healthyMeals << " healthy, "
              << day.unhealthyMeals << " unhealthy (" << day.nutritionPoints << " XP)\n";
    std::cout << "Exercise:     " << (day.exerciseDays > 0 ? "Completed" : "Not completed")
              << " (" << day.exercisePoints << " XP)\n";
    std::cout << "Sleep:        " << day.sleepHours << " / " << day.sleepGoalHours
              << " hours (" << sleepPercent << "%) (" << day.sleepPoints << " XP)\n";
    std::cout << "Daily XP: " << day.totalPoints << "\n\n";
}

void ConsoleUI::showProfile() {
    const ProfileSummary profile = habitService.profileSummary(currentUser->getId());

    std::cout << "\n--- Profile: " << currentUser->getName() << " ---\n";
    std::cout << "XP by habit\n";
    std::cout << "Water:     " << profile.waterXp << " XP\n";
    std::cout << "Nutrition: " << profile.nutritionXp << " XP\n";
    std::cout << "Exercise:  " << profile.exerciseXp << " XP\n";
    std::cout << "Sleep:     " << profile.sleepXp << " XP\n";
    std::cout << "Total:     " << profile.totalXp << " XP\n";
    std::cout << "Level " << profile.levelProgress.level << " ("
              << profile.levelProgress.xpIntoLevel << " / "
              << profile.levelProgress.xpForNextLevel
              << " XP to next level)\n";
    std::cout << "\nCurrent goal streaks (days)\n";
    std::cout << "Water goal met:      " << profile.waterStreak << "\n";
    std::cout << "3 healthy meals:     " << profile.healthyMealsStreak << "\n";
    std::cout << "Exercise completed:  " << profile.exerciseStreak << "\n";
    std::cout << "More than 8h sleep:  " << profile.sleepStreak << "\n\n";
}

void ConsoleUI::showPeriodStats(
    const std::string& period,
    const std::vector<std::string>& dates
) {
    const PeriodSummary summary = habitService.periodSummary(currentUser->getId(), dates);

    if (summary.days == 0) {
        std::cout << "\nNo days to show.\n\n";
        return;
    }

    const int waterPercent = summary.waterGoalMl > 0
        ? summary.consumedWaterMl * 100 / summary.waterGoalMl
        : 0;
    const int sleepPercent = summary.sleepGoalHours > 0
        ? static_cast<int>(summary.sleepHours * 100.0 / summary.sleepGoalHours)
        : 0;

    const std::streamsize oldPrecision = std::cout.precision();
    std::cout << "\n--- " << period << " (" << summary.firstDate
              << " to " << summary.lastDate << ") ---\n";
    std::cout << "Water intake: " << summary.consumedWaterMl << " / " << summary.waterGoalMl
              << " ml (" << waterPercent << "%) (" << summary.waterPoints << " XP)\n";
    std::cout << "Meals:        " << summary.healthyMeals << " healthy, "
              << summary.unhealthyMeals << " unhealthy ("
              << summary.nutritionPoints << " XP)\n";
    std::cout << "Exercise:     " << summary.exerciseDays << " days completed ("
              << summary.exercisePoints << " XP)\n";
    std::cout << "Sleep:        " << std::fixed << std::setprecision(1) << summary.sleepHours
              << " / " << summary.sleepGoalHours << " hours (" << sleepPercent << "%) ("
              << summary.sleepPoints << " XP)\n";
    std::cout << "Total XP: " << summary.totalPoints << "\n\n";
    std::cout.unsetf(std::ios::floatfield);
    std::cout.precision(oldPrecision);
}
