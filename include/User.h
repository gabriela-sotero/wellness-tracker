#pragma once // Prevents multiple inclusions of this header.

#include <optional>
#include <string>

// Account model with private state and read-only getters. Persisted changes go
// through Database; a newly loaded User represents the updated account data.
class User {
private:
    int id;
    std::string username;
    std::string name;
    std::optional<double> weightKg;
    int waterGoalMl;

public:
    // Constructs an account with no stored weight.
    User(
        int id,
        const std::string& username,
        const std::string& name,
        int waterGoalMl
    );

    // Overload used when the account has a recorded weight.
    User(
        int id,
        const std::string& username,
        const std::string& name,
        int waterGoalMl,
        double weightKg
    );

    // Getters expose account state without allowing callers to mutate its fields.
    int getId() const;

    const std::string& getUsername() const;

    const std::string& getName() const;

    const std::optional<double>& getWeightKg() const;

    // Returns the daily water goal in milliliters.
    int getWaterGoalMl() const;
};
