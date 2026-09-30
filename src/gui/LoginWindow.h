#pragma once

#include <QMainWindow>

class QLabel;
class QLineEdit;

class Database;
class User;

class LoginWindow : public QMainWindow {
public:
    explicit LoginWindow(Database& database, QWidget* parent = nullptr);

private:
    Database& database;
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QLabel* feedbackLabel;
    QLineEdit* nameInput;
    QLineEdit* signupUsernameInput;
    QLineEdit* signupPasswordInput;
    QLineEdit* confirmPasswordInput;
    QLabel* signupFeedbackLabel;

    QWidget* createStartPage();
    QWidget* createLoginPage();
    QWidget* createSignupPage();
    void showStartPage();
    void showLoginPage();
    void showSignupPage();
    void attemptLogin();
    void attemptSignup();
    void showWelcomePage(const User& user);
};
