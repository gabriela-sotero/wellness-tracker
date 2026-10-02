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
    src/ConsoleUIMenus.cpp
    src/ConsoleUIAuth.cpp
    src/ConsoleUIHabits.cpp
    src/ConsoleUIProgress.cpp
    src/WaterHabit.cpp
    src/ExerciseHabit.cpp
    src/SleepHabit.cpp
    src/DailyRecord.cpp
    src/NutritionHabit.cpp
    src/HabitService.cpp
)
target_include_directories(wellness_core PUBLIC include)
target_link_libraries(wellness_core PUBLIC ${SQLite3_LIBRARIES})

# CLI app
add_executable(wellness_tracker src/main.cpp)
target_link_libraries(wellness_tracker PRIVATE wellness_core)

# Qt Widgets application. Built by default, so a missing Qt fails at configure
# time instead of silently producing no GUI. Configure with -DBUILD_GUI=OFF to
# build only the core, the CLI and the tests.
option(BUILD_GUI "Build the Qt Widgets application" ON)
if(BUILD_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    add_executable(wellness_gui
        src/gui/main.cpp
        src/gui/LoginWindow.cpp
    )
    # AUTOMOC lets the GUI widgets declare Q_OBJECT and their own signals.
    set_target_properties(wellness_gui PROPERTIES AUTOMOC ON)
    target_include_directories(wellness_gui PRIVATE src/gui)
    target_link_libraries(wellness_gui PRIVATE wellness_core Qt6::Widgets)
endif()

# Unit tests
enable_testing()
add_executable(tests
    tests/test_main.cpp
    tests/test_water.cpp
    tests/test_nutrition.cpp
    tests/test_exercise.cpp
    tests/test_console_stats.cpp
    tests/test_daily_record.cpp
    tests/test_habit_service.cpp
)

target_link_libraries(tests PRIVATE wellness_core)
add_test(NAME unit COMMAND tests)
