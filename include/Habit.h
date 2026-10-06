#pragma once
#include <string>

// Abstract interface shared by all habits. It defines the operations without
// choosing a scoring implementation; subclasses provide polymorphic behavior.
// The virtual destructor allows deleting a subclass through a Habit pointer.
class Habit {
    public:
    virtual ~Habit() = default;
    // Returns points according to the concrete habit's scoring rules.
    virtual int calculateScore() const = 0;
    // Returns the fraction of the goal reached, as calculated by the subclass.
    virtual double progress() const = 0;
    // Returns the habit's human-readable name.
    virtual std::string name() const = 0;
};
