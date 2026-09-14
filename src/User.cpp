#include "User.h"

// No weight is stored when the user chooses not to provide one.
User::User(
    int id,
    const std::string& username,
    const std::string& name,
    int waterGoalMl
)
    : id(id),
      username(username),
      name(name),
      weightKg(std::nullopt),
      waterGoalMl(waterGoalMl) {
}

// Overloaded constructor for users who provide their weight.
User::User(
    int id,
    const std::string& username,
    const std::string& name,
    int waterGoalMl,
    double weightKg
)
    : id(id),
      username(username),
      name(name),
      weightKg(weightKg),
      waterGoalMl(waterGoalMl) {
}

int User::getId() const {
    return id;
}

const std::string& User::getUsername() const {
    return username;
}

const std::string& User::getName() const {
    return name;
}

const std::optional<double>& User::getWeightKg() const {
    return weightKg;
}

int User::getWaterGoalMl() const {
    return waterGoalMl;
}
