#include <iostream>

#include "Database.h"

int main() {
    Database database("data/wellness.db");

    database.createTables();

    int userId = database.insertUser("Gabi");

    if (userId == -1) {
        std::cout << "Failed to create user.\n";
        return 1;
    }

    auto user = database.getUserById(userId);

    if (!user.has_value()) {
        std::cout << "User not found.\n";
        return 1;
    }

    std::cout << "User found:\n";
    std::cout << "ID: " << user->getId() << '\n';
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