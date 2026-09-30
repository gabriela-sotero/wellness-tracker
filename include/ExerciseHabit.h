#pragma once

#include <string>

#include "Habit.h"

class ExerciseHabit : public Habit {
private:
    bool completed;

public:
    ExerciseHabit();

    int calculateScore() const override;
    double progress() const override;
    std::string name() const override;

    void markCompleted();
    bool isCompleted() const;
};
