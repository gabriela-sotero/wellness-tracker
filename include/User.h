#pragma once // Prevents multiple inclusions of this header.

#include <optional>
#include <string>

class User {
private:
    int id;
    std::string username;
    std::string name;
    std::optional<double> weightKg;
    int waterGoalMl;

public:
    // Username and name are passed by const reference to avoid unnecessary string copies.
    User(
        int id,
        const std::string& username,
        const std::string& name,
        int waterGoalMl
    );

    // Overloaded constructor used when the user also provides a weight.
    User(
        int id,
        const std::string& username,
        const std::string& name,
        int waterGoalMl,
        double weightKg
    );

    int getId() const;

    // First const protects the returned string; second const prevents this method from modifying the User.
    const std::string& getUsername() const;


    const std::string& getName() const;

    // First const protects the returned optional; second const prevents this method from modifying the User.
    const std::optional<double>& getWeightKg() const;

    // Daily water goal in ml (defaults to Constants::DEFAULT_WATER_GOAL_ML at registration).
    int getWaterGoalMl() const;
};
