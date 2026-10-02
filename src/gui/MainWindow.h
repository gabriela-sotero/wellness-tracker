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
    QLabel* label = nullptr;
    QProgressBar* bar = nullptr;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // Pages are added to the stack in this order.
    enum Page {
        StartPage,
        LoginPage,
        SignupPage,
        HomePage,
        HabitsPage,
        ProgressPage,
        ProfilePage
    };

    Database& database;
    HabitService habitService;
    std::optional<User> currentUser;   // set while a user is logged in

    QStackedWidget* pages = nullptr;

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

    QLineEdit* waterInput = nullptr;
    QLineEdit* sleepInput = nullptr;
    QLabel* habitFeedback = nullptr;

    QLabel* homeGreeting = nullptr;

    QLabel* profileName = nullptr;
    QLabel* profileMemberSince = nullptr;
    std::vector<QLabel*> badgeImages;
    std::vector<QLabel*> badgeCaptions;
    std::vector<QLabel*> xpValues;
    std::vector<QLabel*> streakValues;

    QLabel* progressHeading = nullptr;
    ProgressRow waterRow;
    ProgressRow mealsRow;
    ProgressRow exerciseRow;
    ProgressRow sleepRow;
    QLabel* progressTotal = nullptr;

    QWidget* createStartPage();
    QWidget* createLoginPage();
    QWidget* createSignupPage();
    QWidget* createHomePage();
    QWidget* createHabitsPage();
    QWidget* createProgressPage();
    QWidget* createProfilePage();

    void showPage(Page page);

    // Logged-out actions.
    void attemptLogin();
    void attemptSignup();

    // Logged-in actions.
    void logWater();
    void logMeal(bool healthy);
    void logExercise();
    void logSleep();
    // toDate marks a period that is still filling up, like the current month.
    void showPeriod(
        const QString& period,
        const std::vector<std::string>& dates,
        bool toDate = false
    );
    void showProfile();
    void logOut();

public:
    explicit MainWindow(Database& database, QWidget* parent = nullptr);
};
