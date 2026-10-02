# Wellness Tracker

Wellness Tracker is a C++ project for tracking daily wellness habits with simple gamification.

The project is being developed for an Object-Oriented Data Structures course, with focus on:

- Object-oriented programming
- Inheritance and polymorphism
- Manually implemented data structures
- Data persistence
- Clean separation between core logic and user interface
- Console and Qt Widgets interfaces

## Tracked Habits

The initial version includes:

- Water intake
- Meals
- Sleep
- Exercise

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

## Requirements

- A C++20 compiler (GCC 13+ or Clang 16+)
- CMake 3.16 or newer
- SQLite 3 **development** package (the headers, not just the runtime library)
- Qt 6 Widgets (required by default; pass `-DBUILD_GUI=OFF` to build only the core, CLI and tests)

On Debian / Ubuntu / Pop!_OS:

```bash
sudo apt install -y build-essential cmake libsqlite3-dev
```

To build the graphical app on Debian / Ubuntu / Pop!_OS, also install Qt Widgets:

```bash
sudo apt install -y qt6-base-dev
```

On Fedora:

```bash
sudo dnf install -y gcc-c++ cmake sqlite-devel
```

For the graphical app, also install Qt Widgets:

```bash
sudo dnf install -y qt6-qtbase-devel
```

On macOS (Homebrew):

```bash
brew install cmake sqlite qt
```

## Build

The `build/` directory is not tracked by git, so configure it once after cloning:

```bash
cmake -S . -B build
```

Then compile (repeat this step after every change):

```bash
cmake --build build
```

## Run the console app

```bash
./build/wellness_tracker
```

## Run the graphical app

Install Qt 6 Widgets for your operating system, then configure the project. CMake checks for Qt during configuration, so repeat this step if you install Qt after configuring:

```bash
cmake -S . -B build
```

Build and launch the GUI:

```bash
cmake --build build --target wellness_gui
./build/wellness_gui
```

The first screen offers **Log in** and **Create an account**. The GUI stores accounts in the same `data/wellness.db` database as the console app.

## Tests

```bash
cmake --build build && ./build/tests
```

Or through CTest:

```bash
ctest --test-dir build --output-on-failure
```

## Troubleshooting

- `Error: .../build is not a directory` — you skipped the configure step. Run `cmake -S . -B build` first.
- `Could NOT find SQLite3 (missing: SQLite3_INCLUDE_DIR SQLite3_LIBRARY)` — the SQLite 3 headers are missing. Install `libsqlite3-dev` (see Requirements) and configure again.
- `Could NOT find Qt6 (missing: Qt6_DIR)` — Qt 6 Widgets is missing. Install it (see Requirements) and configure again, or skip the graphical app with `cmake -S . -B build -DBUILD_GUI=OFF`.
- To start from scratch, delete the directory and reconfigure: `rm -rf build && cmake -S . -B build`.
