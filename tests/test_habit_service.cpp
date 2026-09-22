#include <cassert>
#include <optional>
#include <string>

#include "Constants.h"
#include "Database.h"
#include "DateUtils.h"
#include "HabitService.h"

// Logging water through the service accumulates the daily total and scores it.
static void test_log_water_accumulates_and_scores() {
    Database db(":memory:");
    db.createTables();

    int userId = db.insertUser(
        "alice", "Alice", "pw", std::nullopt, 2000);
    assert(userId > 0);

    HabitService service(db);
    std::string date = util::today();

    // A fresh day starts empty.
    assert(service.consumedWaterMl(userId, date) == 0);
    assert(service.dailyScore(userId, date) == 0);

    // Two logs accumulate into the same day.
    service.logWaterHabit(userId, 1000);
    service.logWaterHabit(userId, 500);

    assert(service.consumedWaterMl(userId, date) == 1500);
    // 1500 / 2000 = 75% of WATER_MAX_SCORE.
    assert(service.dailyScore(userId, date)
           == (Constants::WATER_MAX_SCORE * 3) / 4);
}

// Authentication succeeds only with the right username and password.
static void test_authenticate() {
    Database db(":memory:");
    db.createTables();
    db.insertUser("bob", "Bob", "secret");

    assert(db.authenticate("bob", "secret").has_value());
    assert(!db.authenticate("bob", "wrong").has_value());
    assert(!db.authenticate("ghost", "secret").has_value());
}

void runHabitServiceTests() {
    test_log_water_accumulates_and_scores();
    test_authenticate();
}
