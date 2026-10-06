#pragma once

#include <string>

#include "Habit.h"

// Binary habit: exercise for the day is either completed or not completed.
class ExerciseHabit : public Habit {
private:
    bool completed;

public:
    // A new day starts as not completed.
    ExerciseHabit();

    // Concrete overrides of Habit's virtual interface.
    int calculateScore() const override;
    double progress() const override;
    std::string name() const override;

    // Marks exercise as completed; the model does not provide an undo operation.
    void markCompleted();
    bool isCompleted() const;
};
