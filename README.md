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

## Requirements

- A C++20 compiler (GCC 13+ or Clang 16+)
- CMake 3.16 or newer
- SQLite 3 **development** package (the headers, not just the runtime library)

On Debian / Ubuntu / Pop!_OS:

```bash
sudo apt install -y build-essential cmake libsqlite3-dev
```

On Fedora:

```bash
sudo dnf install -y gcc-c++ cmake sqlite-devel
```

On macOS (Homebrew):

```bash
brew install cmake sqlite
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

## Run

```bash
./build/wellness_tracker
```

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
- To start from scratch, delete the directory and reconfigure: `rm -rf build && cmake -S . -B build`.
