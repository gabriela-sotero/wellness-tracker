# Wellness Tracker

Wellness Tracker is a C++ project for tracking daily wellness habits with simple gamification.

The project is being developed for an Object-Oriented Data Structures course, with focus on:

- Object-oriented programming
- Inheritance and polymorphism
- Manually implemented data structures
- Data persistence
- Clean separation between core logic and user interface
- Future migration to Qt

## Tracked Habits

The initial version includes:

- Water intake
- Sleep
- Exercise
- Social activity

## Planned Features

- User registration and login
- Daily habit tracking
- Daily score
- History
- Weekly summary
- Habit streaks
- Groups
- Weekly ranking
- Local data persistence

## Project Structure

```text
wellness-tracker/
├── CMakeLists.txt
├── .gitignore
├── README.md
├── docs/
│   └── specification.md
├── data/
│   └── .gitkeep
├── include/
├── src/
└── tests/
```

## Documentation

The current project specification is available in:

```text
docs/specification.md
```

## Status

🚧 In development.

The project is currently in its initial structure and core modeling phase.

## Build

You'll need SQLite 3. 
`cmake --build build && ./build/tests`

## Tests

Run unit tests with `cmake --build build && ./build/tests`

