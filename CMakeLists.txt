cmake_minimum_required(VERSION 3.16)

project(wellness_tracker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(wellness_tracker
    src/main.cpp
    src/User.cpp
    src/Database.cpp
)

target_include_directories(wellness_tracker PRIVATE include)

find_package(SQLite3 REQUIRED)

target_link_libraries(wellness_tracker PRIVATE SQLite3::SQLite3)