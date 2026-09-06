#include <iostream>

#include "Database.h"

int main() {
    Database database("data/wellness.db");

    database.createTables();

    int userId = database.insertUser(
        "gabi",
        "Gabi"
    );

    if (userId == -1) {
        std::cout << "Failed to create user.\n";
        return 1;
    }

    auto user = database.getUserByUsername("gabi");

    if (!user.has_value()) {
        std::cout << "User not found.\n";
        return 1;
    }

    std::cout << "User found:\n";
    std::cout << "ID: " << user->getId() << '\n';
    std::cout << "Username: " << user->getUsername() << '\n';
    std::cout << "Name: " << user->getName() << '\n';

    if (user->getWeightKg().has_value()) {
        std::cout << "Weight: "
                  << user->getWeightKg().value()
                  << " kg\n";
    } else {
        std::cout << "Weight: not registered\n";
    }

    return 0;
}