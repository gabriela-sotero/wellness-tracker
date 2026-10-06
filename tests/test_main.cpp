#include <iostream>

// Central runner: each test module exposes one function to execute its cases.
// These functions are implemented in the other test files.
void runWaterTests();
void runNutritionTests();
void runExerciseTests();
void runConsoleStatsTests();
void runDailyRecordTests();
void runHabitServiceTests();

int main() {
    runWaterTests();
    runNutritionTests();
    runExerciseTests();
    runConsoleStatsTests();
    runDailyRecordTests();
    runHabitServiceTests();

    // Reached only if no assertion fails, when assertions are enabled.
    std::cout << "All tests passed\n";
    return 0;
}
