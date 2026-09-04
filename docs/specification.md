# Wellness Tracker Specification

## Overview

Wellness Tracker is a C++ application for tracking daily wellness habits with simple gamification.

Initial habits:

- Water intake
- Sleep
- Exercise
- Social activity

The project focuses on object-oriented programming, manual data structures, persistence, and future migration to Qt.

## Core Features

### User
- Create account
- Login
- Logout
- Edit profile
- Optional weight registration to personalize the water goal

Login requires only:

- Username
- Password

Weight is not required for authentication.

If the user does not provide a weight, the system uses a default water goal of 2000 ml/day.

### Habit Tracking
- Log water intake
- Log sleep
- Mark exercise as completed
- Mark social activity as completed

### Progress
- Daily score
- Daily history
- Weekly summary
- Habit streaks

### Social
- Create or join groups
- View weekly group ranking

## Main Entities

- User
- Habit
- WaterHabit
- SleepHabit
- ExerciseHabit
- SocialHabit
- DailyRecord
- Group

## Habit Hierarchy

```text
Habit
├── WaterHabit
├── SleepHabit
├── ExerciseHabit
└── SocialHabit
```

Each habit may implement its own behavior, such as:

```cpp
virtual int calculatePoints() const = 0;
virtual bool isCompleted() const = 0;
```

## Scoring

Initial score proposal:

| Habit | Points |
|---|---:|
| Water | 25 |
| Sleep | 25 |
| Exercise | 25 |
| Social | 25 |
| Total | 100 |

## Persistence

Data must persist between executions.

The first version may use local files to store:

- Users
- Daily records
- Groups
- Progress

Persistence logic should remain separate from the habit classes.

## Data Structures

Relevant data structures should be implemented manually when required by the course.

Possible uses:

- User storage
- Daily history
- Group members
- Ranking

## Interface

The first version may use a terminal interface.

The core logic must remain independent from the interface so Qt can be added later without rewriting the application logic.

## Scope

### Must Have

- User accounts
- Four habit types
- Daily records
- Daily score
- History
- Persistence
- Inheritance and polymorphism
- Manual data structures

### Should Have

- Weekly summary
- Streaks
- Groups
- Ranking

### Could Have

- Achievements
- Custom goals
- Qt interface
