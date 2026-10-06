#include "ExerciseHabit.h"

#include "Constants.h"

ExerciseHabit::ExerciseHabit()
    : completed(false) {
}

int ExerciseHabit::calculateScore() const {
    // A completed day receives the full score; a missed day receives zero.
    return completed ? Constants::EXERCISE_MAX_SCORE : 0;
}

double ExerciseHabit::progress() const {
    // Binary progress matches the binary completion state.
    return completed ? 1.0 : 0.0;
}

std::string ExerciseHabit::name() const {
    return "Exercise Habit";
}

void ExerciseHabit::markCompleted() {
    completed = true;
}

bool ExerciseHabit::isCompleted() const {
    return completed;
}
