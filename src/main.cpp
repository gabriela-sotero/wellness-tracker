#include <iostream>
#include "User.h"

// Handles both possible states of the optional weight.
void printWeight(const User& user) {
    auto weight = user.getWeightKg();

    if (weight.has_value()) {
        std::cout << weight.value() << " kg\n";
    } else {
        std::cout << "Weight not registered\n";
    }
}

int main() {
    User userWithoutWeight(1, "Gabi");
    User userWithWeight(2, "Alexandre", 70);

    std::cout << userWithoutWeight.getId() << '\n';
    std::cout << userWithoutWeight.getName() << '\n';
    printWeight(userWithoutWeight);

    std::cout << userWithWeight.getId() << '\n';
    std::cout << userWithWeight.getName() << '\n';
    printWeight(userWithWeight);

    return 0;
}