#include "MainWindow.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "Constants.h"
#include "PageLayout.h"

QWidget* MainWindow::createLoginPage() {
    // Create the login controls once; signal handlers call attemptLogin later.
    auto* page = new QWidget;
    auto* layout = startPage(page, "Welcome back");

    loginUsername = addField(layout, "Username");
    loginPassword = addField(layout, "Password", true);
    loginFeedback = addFeedback(layout);

    auto* signIn = new QPushButton("Sign in", page);
    auto* back = new QPushButton("Back", page);
    layout->addWidget(signIn);
    layout->addWidget(back);

    connect(signIn, &QPushButton::clicked, this, [this] {
        attemptLogin();
    });
    connect(loginPassword, &QLineEdit::returnPressed, this, [this] {
        attemptLogin();
    });
    connect(back, &QPushButton::clicked, this, [this] {
        loginFeedback->clear();
        showPage(StartPage);
    });

    return page;
}

QWidget* MainWindow::createSignupPage() {
    // Signup page construction is separate from validation and account creation.
    auto* page = new QWidget;
    auto* layout = startPage(page, "Create your account");

    signupName = addField(layout, "Name");
    signupUsername = addField(layout, "Username");
    signupPassword = addField(layout, "Password", true);
    signupConfirmation = addField(layout, "Confirm password", true);
    signupWeight = addField(layout, "Weight in kg (optional)");
    signupWaterGoal = addField(layout, "Daily water goal in ml (optional)");
    signupFeedback = addFeedback(layout);

    auto* create = new QPushButton("Create account", page);
    auto* back = new QPushButton("Back", page);
    layout->addWidget(create);
    layout->addWidget(back);

    connect(create, &QPushButton::clicked, this, [this] {
        attemptSignup();
    });
    connect(back, &QPushButton::clicked, this, [this] {
        signupFeedback->clear();
        showPage(StartPage);
    });

    return page;
}

void MainWindow::attemptLogin() {
    // Validate locally and update session state only after credential checking succeeds.
    const QString username = loginUsername->text().trimmed();
    const QString password = loginPassword->text();

    loginFeedback->setStyleSheet("");

    if (username.isEmpty() || password.isEmpty()) {
        loginFeedback->setText("Enter your username and password.");
        return;
    }

    auto user = database.authenticate(username.toStdString(), password.toStdString());
    if (!user.has_value()) {
        loginFeedback->setText("Username or password is incorrect.");
        loginPassword->clear();
        return;
    }

    currentUser = user;
    loginFeedback->clear();
    homeGreeting->setText(
        QString("Welcome, %1!").arg(QString::fromStdString(user->getName()))
    );
    showPage(HomePage);
}

void MainWindow::attemptSignup() {
    // Validate all form fields at the UI boundary before inserting a user.
    const QString name = signupName->text().trimmed();
    const QString username = signupUsername->text().trimmed();
    const QString password = signupPassword->text();
    const QString confirmation = signupConfirmation->text();
    const QString weightText = signupWeight->text().trimmed();
    const QString waterGoalText = signupWaterGoal->text().trimmed();

    if (name.isEmpty() || username.isEmpty() || password.isEmpty()) {
        signupFeedback->setText("Fill in your name, username and password.");
        return;
    }
    if (password != confirmation) {
        signupFeedback->setText("The passwords do not match.");
        signupConfirmation->clear();
        return;
    }
    if (database.getUserByUsername(username.toStdString()).has_value()) {
        signupFeedback->setText("That username is already in use.");
        return;
    }

    std::optional<double> weightKg;
    if (!weightText.isEmpty()) {
        bool valid = false;
        const double weight = weightText.toDouble(&valid);
        if (!valid || weight <= 0.0) {
            signupFeedback->setText("Enter a positive number for your weight.");
            return;
        }
        weightKg = weight;
    }

    // The goal comes from the weight unless the user typed one.
    int waterGoalMl = Constants::DEFAULT_WATER_GOAL_ML;
    if (!waterGoalText.isEmpty()) {
        bool valid = false;
        waterGoalMl = waterGoalText.toInt(&valid);
        if (!valid || waterGoalMl <= 0) {
            signupFeedback->setText("Enter a positive whole number of ml for your goal.");
            return;
        }
    } else if (weightKg.has_value()) {
        waterGoalMl = static_cast<int>(*weightKg * Constants::WATER_ML_PER_KG);
    }

    const int userId = database.insertUser(
        username.toStdString(),
        name.toStdString(),
        password.toStdString(),
        weightKg,
        waterGoalMl
    );
    if (userId == -1) {
        signupFeedback->setText("Could not create the account. Please try again.");
        return;
    }

    signupName->clear();
    signupUsername->clear();
    signupPassword->clear();
    signupConfirmation->clear();
    signupWeight->clear();
    signupWaterGoal->clear();
    signupFeedback->clear();

    loginUsername->setText(username);
    loginFeedback->setStyleSheet("color: #007c68;");
    loginFeedback->setText("Account created. You can now sign in.");
    showPage(LoginPage);
}
