#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include "ConsoleUI.h"

static void test_daily_view_shows_each_habit_and_points_on_one_line() {
    Database database(":memory:");
    database.createTables();
    ConsoleUI console(database);

    std::istringstream input(
        "1\n"          // Sign up
        "stats_user\n"
        "Stats User\n"
        "pw\n"
        "n\n"          // No weight
        "n\n"          // Keep default water goal
        "2\n"          // Log in
        "stats_user\n"
        "pw\n"
        "1\n1\n1000\n" // Log 1000 ml water
        "1\n2\ny\n"    // Log one healthy meal
        "1\n3\n"        // Complete exercise
        "1\n4\n4\n"    // Log 4 hours sleep
        "2\n"          // View my day
        "0\n"          // Log out
    );
    std::ostringstream output;
    std::streambuf* originalInput = std::cin.rdbuf(input.rdbuf());
    std::streambuf* originalOutput = std::cout.rdbuf(output.rdbuf());

    console.run();

    std::cin.rdbuf(originalInput);
    std::cout.rdbuf(originalOutput);
    std::cin.clear();

    const std::string screen = output.str();
    assert(screen.find("Water intake: 1000 / 2000 ml (50%) (250 points)")
           != std::string::npos);
    assert(screen.find("Meals:        1 healthy, 0 unhealthy (100 points)")
           != std::string::npos);
    assert(screen.find("Exercise:     Completed (300 points)")
           != std::string::npos);
    assert(screen.find("Sleep:        4 / 8 hours (50%) (250 points)")
           != std::string::npos);
    assert(screen.find("Daily points: 900") != std::string::npos);
}

void runConsoleStatsTests() {
    test_daily_view_shows_each_habit_and_points_on_one_line();
}
