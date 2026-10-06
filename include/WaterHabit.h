#pragma once
#include <string>
#include "Habit.h"

// Habit specialization that stores a day's water goal and consumed amount.
class WaterHabit : public Habit {
private:
    int goalMl;
    int consumedMl;
public:
    // Starts with zero consumption and uses the supplied goal.
    WaterHabit(int goalMl);

    // Overrides the polymorphic operations declared by the base interface.
    int calculateScore() const override;
    double progress() const override;
    std::string name() const override;
    
    // Adds an amount to the day's water consumption.
    void drankWater(int ml);
    int getConsumedMl() const;
};
