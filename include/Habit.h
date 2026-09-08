#pragma once
#include <string>

class Habit {
    public:
    virtual ~Habit() = default;
    virtual int calculateScore() const = 0;
    virtual double progress() const = 0;
    virtual std::string name() const = 0;
};