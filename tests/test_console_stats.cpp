#include <cassert>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

#include "ConsoleUI.h"

class ConsoleStreams {
private:
    std::streambuf* originalInput;
    std::streambuf* originalOutput;
    std::ios::iostate originalInputState;

public:
    ConsoleStreams(std::istream& input, std::ostream& output)
        : originalInput(std::cin.rdbuf()),
          originalOutput(std::cout.rdbuf()),
          originalInputState(std::cin.rdstate()) {
        std::cin.rdbuf(input.rdbuf());
        std::cout.rdbuf(output.rdbuf());
    }

    ~ConsoleStreams() {
        std::cin.rdbuf(originalInput);
        std::cout.rdbuf(originalOutput);
        std::cin.clear(originalInputState);
    }
};

static std::string runConsoleScenario(const std::string& commands) {
    Database database(":memory:");
    database.createTables();
    ConsoleUI console(database);

    std::istringstream input(commands);
    std::ostringstream output;
    {
        ConsoleStreams streams(input, output);
        console.run();
    }
    return output.str();
}

static std::string createAndLogIn() {
    return
        "1\n"          // Sign up
        "stats_user\n"
        "Stats User\n"
        "pw\n"
        "n\n"          // No weight
        "n\n"          // Keep default water goal
        "2\n"          // Log in
        "stats_user\n"
        "pw\n";
}

static void test_daily_and_weekly_views_show_summed_xp() {
    const std::string screen = runConsoleScenario(createAndLogIn() +
        "1\n1\n1000\n" // Log 1000 ml water
        "1\n2\ny\n"    // Log one healthy meal
        "1\n3\n"        // Complete exercise
        "1\n4\n4\n"    // Log 4 hours sleep
        "2\n1\n"       // View progress, then daily
        "2\n2\n"       // View progress, then weekly
        "0\n"          // Log out
    );
    assert(screen.find("Water intake: 1000 / 2000 ml (50%) (250 XP)")
           != std::string::npos);
    assert(screen.find("Meals:        1 healthy, 0 unhealthy (100 XP)")
           != std::string::npos);
    assert(screen.find("Exercise:     Completed (300 XP)")
           != std::string::npos);
    assert(screen.find("Sleep:        4 / 8 hours (50%) (250 XP)")
           != std::string::npos);
    assert(screen.find("Daily XP: 900") != std::string::npos);
    assert(screen.find("Water intake: 1000 / 14000 ml (7%) (250 XP)")
           != std::string::npos);
    assert(screen.find("Total XP: 900") != std::string::npos);
}

static void test_profile_shows_xp_totals_and_goal_streaks() {
    const std::string screen = runConsoleScenario(createAndLogIn() +
        "1\n1\n2000\n" // Meet the water goal
        "1\n2\ny\n"    // Three healthy meals
        "1\n2\ny\n"
        "1\n2\ny\n"
        "1\n3\n"        // Complete exercise
        "1\n4\n9\n"    // Sleep more than 8 hours
        "3\n"            // View profile
        "0\n"            // Log out
    );

    assert(screen.find("Water:     500 XP") != std::string::npos);
    assert(screen.find("Nutrition: 300 XP") != std::string::npos);
    assert(screen.find("Exercise:  300 XP") != std::string::npos);
    assert(screen.find("Sleep:     500 XP") != std::string::npos);
    assert(screen.find("Total:     1600 XP") != std::string::npos);
    assert(screen.find("Water goal met:      1") != std::string::npos);
    assert(screen.find("3 healthy meals:     1") != std::string::npos);
    assert(screen.find("Exercise completed:  1") != std::string::npos);
    assert(screen.find("More than 8h sleep:  1") != std::string::npos);
}

void runConsoleStatsTests() {
    test_daily_and_weekly_views_show_summed_xp();
    test_profile_shows_xp_totals_and_goal_streaks();
}
