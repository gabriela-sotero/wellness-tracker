#include <cassert>
#include <optional>
#include <string>
#include <vector>

#include "Constants.h"
#include "Database.h"
#include "DateUtils.h"
#include "DailyRecord.h"
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

// Meal counts and nutrition points persist through the daily record store.
static void test_log_meals_accumulates_and_scores() {
    Database db(":memory:");
    db.createTables();

    int userId = db.insertUser(
        "carol", "Carol", "pw", std::nullopt, 2000);
    assert(userId > 0);

    HabitService service(db);
    std::string date = util::today();

    service.logMealHabit(userId, true);
    service.logMealHabit(userId, false);
    service.logMealHabit(userId, true);

    NutritionSummary meals = service.nutritionSummary(userId, date);
    assert(meals.healthyMeals == 2);
    assert(meals.unhealthyMeals == 1);
    assert(meals.points == (2 * Constants::NUTRITION_MAX_SCORE / 3)
           - Constants::NUTRITION_UNHEALTHY_PENALTY);
    assert(service.dailyScore(userId, date) == meals.points);
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

// Saves a finished day straight to the store, so tests can build history.
static void saveDay(Database& db, int userId, const std::string& date,
                    int waterMl, int healthyMeals, double sleepHours) {
    DailyRecord record(userId, date, 2000);
    record.logWater(waterMl);
    record.seedMeals(healthyMeals, 0);
    record.seedExercise(true);
    record.seedSleep(sleepHours);
    db.saveDailyRecord(record);
}

// A period adds up every day in the range and scales the goals by its length.
static void test_period_summary_adds_up_the_range() {
    Database db(":memory:");
    db.createTables();

    int userId = db.insertUser("dave", "Dave", "pw", std::nullopt, 2000);
    std::vector<std::string> dates = util::lastDays(2);
    saveDay(db, userId, dates.front(), 1000, 1, 8.0);
    saveDay(db, userId, dates.back(), 500, 2, 8.0);

    HabitService service(db);
    PeriodSummary summary = service.periodSummary(userId, dates);

    assert(summary.days == 2);
    assert(summary.firstDate == dates.front());
    assert(summary.lastDate == dates.back());
    assert(summary.consumedWaterMl == 1500);
    assert(summary.waterGoalMl == 4000);       // 2000 ml x 2 days
    assert(summary.waterPoints == 375);        // 250 + 125
    assert(summary.healthyMeals == 3);
    assert(summary.exerciseDays == 2);
    assert(summary.sleepHours == 16.0);
}

// An empty range yields a zeroed summary instead of reading the store.
static void test_period_summary_without_days() {
    Database db(":memory:");
    db.createTables();

    int userId = db.insertUser("erin", "Erin", "pw", std::nullopt, 2000);
    HabitService service(db);

    assert(service.periodSummary(userId, {}).days == 0);
}

// The profile totals XP since the first record and counts the goal streaks.
static void test_profile_summary_totals_xp_and_streaks() {
    Database db(":memory:");
    db.createTables();

    int userId = db.insertUser("frank", "Frank", "pw", std::nullopt, 2000);
    std::vector<std::string> dates = util::lastDays(2);
    saveDay(db, userId, dates.front(), 2000, 3, 9.0);
    saveDay(db, userId, dates.back(), 2000, 3, 9.0);

    HabitService service(db);
    ProfileSummary profile = service.profileSummary(userId);

    assert(profile.waterXp == 1000);           // Goal met on both days.
    assert(profile.exerciseXp == 600);
    assert(profile.totalXp == profile.waterXp + profile.nutritionXp
           + profile.exerciseXp + profile.sleepXp);
    assert(profile.waterStreak == 2);
    assert(profile.healthyMealsStreak == 2);
    assert(profile.exerciseStreak == 2);
    assert(profile.sleepStreak == 2);
}

// A user who never logged anything has no history to summarise.
static void test_profile_summary_without_history() {
    Database db(":memory:");
    db.createTables();

    int userId = db.insertUser("grace", "Grace", "pw", std::nullopt, 2000);
    ProfileSummary profile = HabitService(db).profileSummary(userId);

    assert(profile.totalXp == 0);
    assert(profile.waterStreak == 0);
}

void runHabitServiceTests() {
    test_log_water_accumulates_and_scores();
    test_log_meals_accumulates_and_scores();
    test_authenticate();
    test_period_summary_adds_up_the_range();
    test_period_summary_without_days();
    test_profile_summary_totals_xp_and_streaks();
    test_profile_summary_without_history();
}
