# Wellness Tracker

Wellness Tracker is a personal habit journal built with C++20. It helps you keep a daily record of water, meals, exercise, and sleep, then look back on your routines over time. The app includes a console interface and a Qt Widgets GUI backed by the same local SQLite database.

## Features

- Create an account, sign in, log out, and permanently delete an account with its habit data.
- Keep a daily record of water, meals, exercise, and sleep.
- Review daily, weekly, month-to-date, and year-to-date progress.
- View lifetime XP, habit streaks, and streak badges for water, meals, and exercise.
- Gain XP and levels. Level `n` starts at `XP_LEVEL_BASE × (n − 1)²` total XP; the current base is `35`.
- Get a level-up notification when recording a habit crosses a level threshold.

The console and GUI share `data/wellness.db`. Run either app from the repository root so the database and badge artwork paths resolve correctly.

## Project Structure

```text
wellness-tracker/
├── CMakeLists.txt
├── README.md
├── assets/
│   ├── badges/             # Habit streak badge artwork
│   └── icons/              # GUI icons
├── data/
│   └── .gitkeep             # Runtime SQLite database location
├── docs/
│   ├── img/                 # Project screenshots
│   ├── index.html            # GitHub Pages site
│   ├── report.pdf            # Project report
│   ├── specification.md      # Course scope and design notes
│   └── style.css             # GitHub Pages styles
├── include/                 # Core public headers
├── src/
│   ├── gui/                  # Qt Widgets interface
│   └──                       # Core logic and console interface
├── tests/                    # Automated tests
└── tools/
    ├── generate_badges.py    # Badge artwork generator
    └── seed_demo_profile.cpp # Demo profile seeder
```

## Documentation

Course scope and design notes are available in:

```text
docs/specification.md
```

## Status

🚧 In development. Account management, habit tracking, progress summaries, streak badges, and XP levels are implemented.

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

Run it from the repository root: both the database and the badge artwork in `assets/badges/` are read relative to the working directory.

### Badge artwork

The profile screen shows one streak badge per habit, loaded from `assets/badges/`. Qt reads SVG through the image plugin that ships with Qt base, so no extra module is linked. To change a colour, a tier or an icon, edit `tools/generate_badges.py` and run it:

```bash
python3 tools/generate_badges.py
```

To create a local demo account with a year of habit records that start irregular and improve gradually, while keeping some missed goals, for previewing profile badges and progress statistics:

```bash
cmake --build build --target seed_demo_profile
./build/seed_demo_profile
```

Sign in with username `demo` and password `demo`. The utility adds this account to `data/wellness.db`; if it already exists, it aligns its creation date with the start of the one-year path history without changing habit records. Run `./build/seed_demo_profile --reset` to replace the demo account and regenerate its sample data.

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
