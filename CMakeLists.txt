cmake_minimum_required(VERSION 3.16)

project(wellness_tracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

# Shared logic (no main), reused by the app, tests and future GUI.
add_library(wellness_core
    src/User.cpp
    src/Database.cpp
    src/ConsoleUI.cpp
    src/WaterHabit.cpp
    include/NutritionHabit.h
    src/NutritionHabit.cpp
)
target_include_directories(wellness_core PUBLIC include)
target_link_libraries(wellness_core PUBLIC SQLite3::SQLite3)

# CLI app
add_executable(wellness_tracker src/main.cpp)
target_link_libraries(wellness_tracker PRIVATE wellness_core)

# Unit tests
enable_testing()
add_executable(tests tests/test_main.cpp)
target_link_libraries(tests PRIVATE wellness_core)
add_test(NAME unit COMMAND tests)
