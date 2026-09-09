#include <cassert>
#include <string>

#include "DailyRecord.h"
#include "Constants.h"

// A new record carries its identity and starts with no consumption or points.
static void test_daily_record_initial_state() {
    DailyRecord record(1, "2026-09-09", 2000);

    assert(record.getUserId() == 1);
    assert(record.getDate() == "2026-09-09");
    assert(record.consumedWaterMl() == 0);
    assert(record.dailyScore() == 0);
}

// Logging water delegates to the water habit: the raw intake accumulates
// while the daily score follows the habit's capped scoring.
static void test_daily_record_logs_and_scores_water() {
    constexpr int goalMl = 2000;
    DailyRecord record(1, "2026-09-09", goalMl);

    // Half of the goal earns half of the water maximum.
    record.logWater(goalMl / 2);

    assert(record.consumedWaterMl() == goalMl / 2);
    assert(record.dailyScore() == Constants::WATER_MAX_SCORE / 2);

    // Exceeding the goal keeps accumulating the raw intake, but the score caps.
    record.logWater(5000);

    assert(record.consumedWaterMl() == goalMl / 2 + 5000);
    assert(record.dailyScore() == Constants::WATER_MAX_SCORE);
}

// Exposes one entry point for all daily record tests.
void runDailyRecordTests() {
    test_daily_record_initial_state();
    test_daily_record_logs_and_scores_water();
}
