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

    QWidget* createLoginPage();
    void attemptLogin();
    void showWelcomePage(const User& user);
};
