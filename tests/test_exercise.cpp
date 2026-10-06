#include <cassert>
#include <optional>
#include <string>

#include "Constants.h"
#include "DateUtils.h"
#include "ExerciseHabit.h"
#include "HabitService.h"

// Covers ExerciseHabit state, scoring, and persistence through HabitService.
static void test_exercise_habit_initial_state() {
    ExerciseHabit exercise;

    assert(!exercise.isCompleted());
    assert(exercise.progress() == 0.0);
    assert(exercise.calculateScore() == 0);
    assert(exercise.name() == "Exercise Habit");
}

static void test_exercise_habit_completion() {
    ExerciseHabit exercise;
    exercise.markCompleted();

    assert(exercise.isCompleted());
    assert(exercise.progress() == 1.0);
    assert(exercise.calculateScore() == Constants::EXERCISE_MAX_SCORE);

    // Completion is a daily state, so marking it more than once does not add points.
    exercise.markCompleted();
    assert(exercise.calculateScore() == Constants::EXERCISE_MAX_SCORE);
}

static void test_exercise_logging_persists_and_scores() {
    Database database(":memory:");
    database.createTables();
    int userId = database.insertUser(
        "exercise_user", "Exercise User", "pw", std::nullopt, 2000
    );
    assert(userId > 0);

    HabitService service(database);
    std::string date = util::today();

    ExerciseSummary initial = service.exerciseSummary(userId, date);
    assert(!initial.completed);
    assert(initial.points == 0);
    assert(service.dailyScore(userId, date) == 0);

    service.logExerciseHabit(userId);
    service.logExerciseHabit(userId);

    ExerciseSummary completed = service.exerciseSummary(userId, date);
    assert(completed.completed);
    assert(completed.points == Constants::EXERCISE_MAX_SCORE);
    assert(service.dailyScore(userId, date) == Constants::EXERCISE_MAX_SCORE);
}

void runExerciseTests() {
    test_exercise_habit_initial_state();
    test_exercise_habit_completion();
    test_exercise_logging_persists_and_scores();
}
