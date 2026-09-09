#include <iostream>

// These functions are implemented in the other test files.
void runWaterTests();
void runNutritionTests();
void runDailyRecordTests();

int main() {
    runWaterTests();
    runNutritionTests();
    runDailyRecordTests();

    // Reached only if no assertion fails, when assertions are enabled.
    std::cout << "All tests passed\n";
    return 0;
}