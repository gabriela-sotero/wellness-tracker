# Wellness Tracker Specification

## Overview

Wellness Tracker is a personal, journal-style app for keeping a structured daily record of wellness habits. Users can record water intake, meals, exercise, and sleep, then review their progress over time. The app is written in C++20 and provides both a console interface and a Qt Widgets interface.

## Accounts

- Users can create an account, sign in, and log out.
- Account creation accepts a username, name, and password.
- Users may enter their weight to calculate a daily water goal, or set a custom goal. Without either, the goal defaults to 2000 ml.
- Deleting an account requires confirmation and the current password. The account and its associated habit records are deleted.
- The console app and GUI use the same local SQLite database at `data/wellness.db`.

## Daily Habit Journal

Each user can record the following for a day:

| Habit | Daily record |
|---|---|
| Water | Total water intake in millilitres, compared with the user's water goal |
| Meals | Healthy and unhealthy meal counts |
| Exercise | Whether exercise was completed |
| Sleep | Total hours slept, compared with an 8-hour goal |

Records are stored by user and date. Repeated water, meal, and sleep entries for the same day accumulate; exercise is marked complete for that day.

## Scoring and Levels

Habit scores contribute to daily points and lifetime XP:

| Habit | Scoring rule | Maximum per day |
|---|---|---:|
| Water | Proportional to the daily water goal; capped at the goal | 500 XP |
| Meals | 100 XP per healthy meal, up to three; 50 XP deducted per unhealthy meal, with a minimum score of zero | 300 XP before penalties |
| Exercise | Awarded when exercise is marked complete | 300 XP |
| Sleep | Proportional to the 8-hour goal; capped at the goal | 500 XP |

Lifetime XP is the sum of daily habit scores. The XP threshold to reach level `n` is:

```text
XP_LEVEL_BASE × (n − 1)²
```

`XP_LEVEL_BASE` is currently `35`. The app shows progress toward the next level and reports when recording a habit advances the user to a higher level.

## Progress and Achievements

- Users can review daily, weekly, month-to-date, and year-to-date summaries.
- The profile shows lifetime XP by habit, current streaks, and the longest streaks.
- Streak goals are meeting the water goal, recording at least three healthy meals, completing exercise, and sleeping more than eight hours.
- Water, meals, and exercise streaks unlock profile badges at 3, 7, 15, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, and 365 days.

## Application Structure

- `include/` contains shared model, database, and service interfaces.
- `src/` contains the shared logic and console interface.
- `src/gui/` contains the Qt Widgets interface.
- `assets/badges/` contains the streak badge artwork.
- `tools/generate_badges.py` generates the badge SVG files.
- `tests/` contains the automated tests.

Habit scoring is implemented by the habit classes. `HabitService` coordinates habit and profile operations, while `Database` handles SQLite persistence. The console and graphical interfaces use the same shared services.

## Runtime Requirements

- C++20 compiler
- CMake 3.16 or newer
- SQLite 3 development package
- Qt 6 Widgets for the GUI (configure with `-DBUILD_GUI=OFF` to build the console app and tests without Qt)

Build and launch commands are documented in the repository [README](../README.md).
