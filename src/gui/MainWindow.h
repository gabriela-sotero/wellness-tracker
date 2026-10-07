#pragma once

#include <QMainWindow>
#include <QString>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "Database.h"
#include "HabitService.h"
#include "User.h"

class PathView;
class QDate;
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QScrollArea;
class QStackedWidget;
class QVBoxLayout;

// One habit on the progress page: the name and XP above, the bar below.
struct ProgressRow {
    // The label describes the measurement; the bar visualizes its value.
    QLabel* label = nullptr;
    QLabel* detail = nullptr;
    QProgressBar* bar = nullptr;
};

struct QuestCard {
    QWidget* card = nullptr;
    QVBoxLayout* body = nullptr;
    QLabel* status = nullptr;
    QLabel* badge = nullptr;
    QProgressBar* bar = nullptr;
    QLabel* feedback = nullptr;
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

    // Bottom bar shown on signed-in pages: home, log, profile and sign out.
    // The buttons are owned by the bar; the order matches the icons in createNavBar.
    QWidget* navBar = nullptr;
    std::vector<QPushButton*> navButtons;

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
    QLabel* questsSummary = nullptr;
    QProgressBar* questsBar = nullptr;
    QPushButton* exerciseButton = nullptr;
    QuestCard waterQuest;
    QuestCard mealQuest;
    QuestCard exerciseQuest;
    QuestCard sleepQuest;

    // Home header and day path are refreshed from the services whenever home opens.
    QLabel* homeLevel = nullptr;
    std::vector<QLabel*> homeHabitStreaks;
    QLabel* homeXp = nullptr;
    QScrollArea* homeScroll = nullptr;
    PathView* homePath = nullptr;
    // Guards against re-entering the scroll handler while history is being added.
    bool extendingPath = false;

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
    // Period selector buttons; the one matching the shown period is highlighted.
    std::vector<QPushButton*> periodTabs;

    // Page factories build each screen once; navigation reuses those widgets.
    QWidget* createStartPage();
    QWidget* createLoginPage();
    QWidget* createSignupPage();
    QWidget* createHomePage();
    QWidget* createHabitsPage();
    QWidget* createProgressPage();
    QWidget* createProfilePage();

    // Builds the bottom navigation bar once; showPage toggles its visibility.
    QWidget* createNavBar();

    // Selects a page in the stack by its enum value.
    void showPage(Page page);
    // Highlights the nav button that owns the page being shown.
    void updateNav(Page page);
    // Reloads the header stats and resets the path so today is in view.
    void refreshHome();
    // Loads more history when the path is scrolled to the top, and more locked
    // days when it is scrolled to the bottom.
    void extendPathAtEdges(int scrollValue);
    // How many of the four daily goals the signed-in user met on a date, or -1
    // when the service has nothing for it.
    int goalsMetOn(const QDate& date);

    // Logged-out actions.
    // Validate form input before asking Database to create or authenticate an account.
    void attemptLogin();
    void attemptSignup();

    // Logged-in actions.
    // Record actions delegate business rules to HabitService.
    void logWater();
    void addWater(int ml);
    void addSleep(double hours);
    void logMeal(bool healthy);
    void logExercise();
    void logSleep();
    void refreshHabits();
    void logHabit(QuestCard& quest, const QString& message, const std::function<void()>& record);
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
