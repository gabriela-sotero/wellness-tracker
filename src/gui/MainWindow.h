#pragma once

#include <QMainWindow>
#include <QString>
#include <optional>
#include <string>
#include <vector>

#include "Database.h"
#include "HabitService.h"
#include "User.h"

class QLabel;
class QLineEdit;
class QProgressBar;
class QStackedWidget;
class QVBoxLayout;

// One habit on the progress page: the name and XP above, the bar below.
struct ProgressRow {
    // The label describes the measurement; the bar visualizes its value.
    QLabel* label = nullptr;
    QProgressBar* bar = nullptr;
};

// Main Qt window. QMainWindow supplies the window behavior; pages keeps each
// screen separate in a QStackedWidget. Database is borrowed, HabitService is
// composed, and currentUser stores the authenticated session state.
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // These enum values match the order in which pages enter the stack.
    enum Page {
        StartPage,
        LoginPage,
        SignupPage,
        HomePage,
        HabitsPage,
        ProgressPage,
        ProfilePage
    };

    // Non-owning reference: main creates and destroys the Database instance.
    Database& database;
    // Service uses the same connection to centralize habit rules.
    HabitService habitService;
    // Empty while logged out; populated after successful authentication.
    std::optional<User> currentUser;

    // Owns the screen widgets and switches which page is visible.
    QStackedWidget* pages = nullptr;

    // Non-owning pointers to controls owned by their respective page widgets.
    // Authentication controls are stored here so event handlers can read or clear them.
    QLineEdit* loginUsername = nullptr;
    QLineEdit* loginPassword = nullptr;
    QLabel* loginFeedback = nullptr;

    QLineEdit* signupName = nullptr;
    QLineEdit* signupUsername = nullptr;
    QLineEdit* signupPassword = nullptr;
    QLineEdit* signupConfirmation = nullptr;
    QLineEdit* signupWeight = nullptr;
    QLineEdit* signupWaterGoal = nullptr;
    QLabel* signupFeedback = nullptr;

    // Habit-entry controls feed validated values into HabitService.
    QLineEdit* waterInput = nullptr;
    QLineEdit* sleepInput = nullptr;
    QLabel* habitFeedback = nullptr;

    // Home greeting is filled after a user authenticates.
    QLabel* homeGreeting = nullptr;

    // Profile labels and collections are refreshed from ProfileSummary on entry.
    QLabel* profileName = nullptr;
    QLabel* profileMemberSince = nullptr;
    QLabel* profileWaterGoal = nullptr;
    QLabel* profileWeight = nullptr;
    QWidget* profileWeightRow = nullptr;
    QLabel* levelProgressLabel = nullptr;
    QProgressBar* levelProgressBar = nullptr;
    std::vector<QLabel*> badgeImages;
    std::vector<QLabel*> badgeCaptions;
    std::vector<QLabel*> xpValues;
    std::vector<QLabel*> streakValues;

    // Progress page widgets are updated from a period summary when a range is selected.
    QLabel* progressHeading = nullptr;
    QLabel* progressOverall = nullptr;
    QProgressBar* progressOverallBar = nullptr;
    ProgressRow waterRow;
    ProgressRow mealsRow;
    ProgressRow exerciseRow;
    ProgressRow sleepRow;
    QLabel* progressTotal = nullptr;

    // Page factories build each screen once; navigation reuses those widgets.
    QWidget* createStartPage();
    QWidget* createLoginPage();
    QWidget* createSignupPage();
    QWidget* createHomePage();
    QWidget* createHabitsPage();
    QWidget* createProgressPage();
    QWidget* createProfilePage();

    // Selects a page in the stack by its enum value.
    void showPage(Page page);

    // Logged-out actions.
    // Validate form input before asking Database to create or authenticate an account.
    void attemptLogin();
    void attemptSignup();

    // Logged-in actions.
    // Record actions delegate business rules to HabitService.
    void logWater();
    void logMeal(bool healthy);
    void logExercise();
    void logSleep();
    // Compares the current level with the level captured before a habit action.
    QString levelUpMessage(int previousLevel) const;
    // toDate marks a period that is still filling up, like the current month.
    void showPeriod(
        const QString& period,
        const std::vector<std::string>& dates,
        bool toDate = false
    );
    void showProfile();
    void editDailyWaterGoal();
    void editWeight();
    void logOut();

public:
    // The window borrows its database from the caller.
    explicit MainWindow(Database& database, QWidget* parent = nullptr);
};
