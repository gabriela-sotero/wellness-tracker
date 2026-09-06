#pragma once // Prevents multiple inclusions of this header.

#include <optional>
#include <string>

class User {
private:
    int id;
    std::string name;
    std::optional<double> weightKg;

public:
    // Name is passed by const reference to avoid an unnecessary string copy.
    User(int id, const std::string& name);

    // Overloaded constructor used when the user also provides a weight.
    User(int id, const std::string& name, double weightKg);

    int getId() const;

    // First const protects the returned string; second const prevents this method from modifying the User.
    const std::string& getName() const;

    // First const protects the returned optional; second const prevents this method from modifying the User.
    const std::optional<double>& getWeightKg() const;
};