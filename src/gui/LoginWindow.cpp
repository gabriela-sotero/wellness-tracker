#include "LoginWindow.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "Database.h"
#include "User.h"

LoginWindow::LoginWindow(Database& database, QWidget* parent)
    : QMainWindow(parent),
      database(database),
      usernameInput(nullptr),
      passwordInput(nullptr),
      feedbackLabel(nullptr),
      nameInput(nullptr),
      signupUsernameInput(nullptr),
      signupPasswordInput(nullptr),
      confirmPasswordInput(nullptr),
      signupFeedbackLabel(nullptr) {
    setWindowTitle("Wellness Tracker");
    setMinimumSize(560, 620);
    resize(620, 680);
    setStyleSheet(R"(
        QMainWindow, QWidget#startPage, QWidget#loginPage,
        QWidget#signupPage, QWidget#welcomePage {
            background-color: #f4f7f5;
        }
        QLabel#brand {
            color: #16856b;
            font-size: 13px;
            font-weight: 700;
            letter-spacing: 2px;
        }
        QLabel#title {
            color: #183b35;
            font-size: 30px;
            font-weight: 700;
        }
        QLabel#subtitle {
            color: #66817a;
            font-size: 14px;
        }
        QLineEdit {
            min-height: 42px;
            padding: 0 12px;
            border: 1px solid #d5e2dd;
            border-radius: 9px;
            background-color: white;
            color: #183b35;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 2px solid #16856b;
        }
        QPushButton {
            min-height: 46px;
            border: none;
            border-radius: 9px;
            background-color: #16856b;
            color: white;
            font-size: 15px;
            font-weight: 600;
        }
        QPushButton:hover { background-color: #116d58; }
        QLabel#feedback { color: #b54747; font-size: 13px; }
    )");

    setCentralWidget(createStartPage());
}

QWidget* LoginWindow::createStartPage() {
    auto* page = new QWidget(this);
    page->setObjectName("startPage");
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 48, 30, 42);
    layout->setSpacing(14);

    auto* brand = new QLabel("WELLNESS TRACKER", page);
    brand->setObjectName("brand");
    layout->addWidget(brand);

    auto* title = new QLabel("Small steps,\nstronger you.", page);
    title->setObjectName("title");
    layout->addWidget(title);

    auto* subtitle = new QLabel("Build healthy habits, one day at a time.", page);
    subtitle->setObjectName("subtitle");
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);
    layout->addStretch();

    auto* actions = new QWidget(page);
    actions->setFixedWidth(480);
    auto* actionsLayout = new QVBoxLayout(actions);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->setSpacing(12);

    auto* loginButton = new QPushButton("Log in", actions);
    auto* signupButton = new QPushButton("Create an account", actions);
    actionsLayout->addWidget(loginButton);
    actionsLayout->addWidget(signupButton);
    layout->addWidget(actions, 0, Qt::AlignHCenter);

    connect(loginButton, &QPushButton::clicked, this, [this] {
        showLoginPage();
    });
    connect(signupButton, &QPushButton::clicked, this, [this] {
        showSignupPage();
    });
    return page;
}

void LoginWindow::showStartPage() {
    QWidget* oldPage = takeCentralWidget();
    usernameInput = nullptr;
    passwordInput = nullptr;
    feedbackLabel = nullptr;
    nameInput = nullptr;
    signupUsernameInput = nullptr;
    signupPasswordInput = nullptr;
    confirmPasswordInput = nullptr;
    signupFeedbackLabel = nullptr;
    setCentralWidget(createStartPage());
    if (oldPage != nullptr) {
        oldPage->deleteLater();
    }
}

void LoginWindow::showLoginPage() {
    QWidget* oldPage = takeCentralWidget();
    nameInput = nullptr;
    signupUsernameInput = nullptr;
    signupPasswordInput = nullptr;
    confirmPasswordInput = nullptr;
    signupFeedbackLabel = nullptr;
    setCentralWidget(createLoginPage());
    if (oldPage != nullptr) {
        oldPage->deleteLater();
    }
}

void LoginWindow::showSignupPage() {
    QWidget* oldPage = takeCentralWidget();
    usernameInput = nullptr;
    passwordInput = nullptr;
    feedbackLabel = nullptr;
    setCentralWidget(createSignupPage());
    if (oldPage != nullptr) {
        oldPage->deleteLater();
    }
}

QWidget* LoginWindow::createLoginPage() {
    auto* page = new QWidget(this);
    page->setObjectName("loginPage");
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 48, 30, 42);
    layout->setSpacing(14);

    auto* brand = new QLabel("WELLNESS TRACKER", page);
    brand->setObjectName("brand");
    layout->addWidget(brand);

    auto* title = new QLabel("Welcome back", page);
    title->setObjectName("title");
    layout->addWidget(title);

    auto* subtitle = new QLabel("Sign in to continue your wellness journey.", page);
    subtitle->setObjectName("subtitle");
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);
    layout->addSpacing(18);

    auto* backButton = new QPushButton("← Back", page);
    backButton->setMinimumWidth(140);
    layout->addWidget(backButton);

    auto* formPanel = new QWidget(page);
    formPanel->setFixedWidth(480);
    auto* form = new QVBoxLayout(formPanel);
    form->setContentsMargins(0, 0, 0, 0);
    form->setSpacing(8);

    auto* usernameLabel = new QLabel("Username", formPanel);
    form->addWidget(usernameLabel);
    usernameInput = new QLineEdit(formPanel);
    usernameInput->setPlaceholderText("Username");
    usernameInput->setAccessibleName("Username");
    form->addWidget(usernameInput);

    auto* passwordLabel = new QLabel("Password", formPanel);
    form->addWidget(passwordLabel);
    passwordInput = new QLineEdit(formPanel);
    passwordInput->setPlaceholderText("Password");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setAccessibleName("Password");
    form->addWidget(passwordInput);
    layout->addSpacing(8);

    auto* signInButton = new QPushButton("Sign in", formPanel);
    signInButton->setDefault(true);
    form->addWidget(signInButton);

    feedbackLabel = new QLabel(formPanel);
    feedbackLabel->setObjectName("feedback");
    feedbackLabel->setWordWrap(true);
    form->addWidget(feedbackLabel);
    layout->addWidget(formPanel, 0, Qt::AlignHCenter);
    layout->addStretch();

    connect(signInButton, &QPushButton::clicked, this, [this] {
        attemptLogin();
    });
    connect(passwordInput, &QLineEdit::returnPressed, this, [this] {
        attemptLogin();
    });
    connect(backButton, &QPushButton::clicked, this, [this] {
        showStartPage();
    });

    return page;
}

QWidget* LoginWindow::createSignupPage() {
    auto* page = new QWidget(this);
    page->setObjectName("signupPage");
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 36, 30, 36);
    layout->setSpacing(12);

    auto* brand = new QLabel("WELLNESS TRACKER", page);
    brand->setObjectName("brand");
    layout->addWidget(brand);

    auto* title = new QLabel("Create your account", page);
    title->setObjectName("title");
    title->setWordWrap(true);
    layout->addWidget(title);

    auto* subtitle = new QLabel("Start tracking your daily habits.", page);
    subtitle->setObjectName("subtitle");
    layout->addWidget(subtitle);

    auto* formPanel = new QWidget(page);
    formPanel->setFixedWidth(480);
    auto* form = new QVBoxLayout(formPanel);
    form->setContentsMargins(0, 0, 0, 0);
    form->setSpacing(6);

    const auto addField = [form](const QString& labelText, const QString& placeholder,
                                 QLineEdit*& field, QWidget* parent, bool password = false) {
        auto* label = new QLabel(labelText, parent);
        field = new QLineEdit(parent);
        field->setPlaceholderText(placeholder);
        field->setAccessibleName(labelText);
        if (password) {
            field->setEchoMode(QLineEdit::Password);
        }
        form->addWidget(label);
        form->addWidget(field);
    };

    addField("Name", "Your name", nameInput, formPanel);
    addField("Username", "Choose a username", signupUsernameInput, formPanel);
    addField("Password", "Create a password", signupPasswordInput, formPanel, true);
    addField("Confirm password", "Enter the password again", confirmPasswordInput,
             formPanel, true);

    signupFeedbackLabel = new QLabel(formPanel);
    signupFeedbackLabel->setObjectName("feedback");
    signupFeedbackLabel->setWordWrap(true);
    form->addWidget(signupFeedbackLabel);

    auto* createAccountButton = new QPushButton("Create account", formPanel);
    createAccountButton->setDefault(true);
    form->addWidget(createAccountButton);
    layout->addWidget(formPanel, 0, Qt::AlignHCenter);

    auto* backButton = new QPushButton("Back to start", page);
    backButton->setMinimumWidth(200);
    layout->addWidget(backButton, 0, Qt::AlignHCenter);
    layout->addStretch();

    connect(createAccountButton, &QPushButton::clicked, this, [this] {
        attemptSignup();
    });
    connect(confirmPasswordInput, &QLineEdit::returnPressed, this, [this] {
        attemptSignup();
    });
    connect(backButton, &QPushButton::clicked, this, [this] {
        showStartPage();
    });
    return page;
}

void LoginWindow::attemptLogin() {
    const QString username = usernameInput->text().trimmed();
    const QString password = passwordInput->text();

    if (username.isEmpty() || password.isEmpty()) {
        feedbackLabel->setStyleSheet("color: #b54747;");
        feedbackLabel->setText("Enter your username and password.");
        return;
    }

    const auto user = database.authenticate(
        username.toStdString(),
        password.toStdString()
    );
    if (!user.has_value()) {
        feedbackLabel->setStyleSheet("color: #b54747;");
        feedbackLabel->setText("Username or password is incorrect.");
        passwordInput->clear();
        passwordInput->setFocus();
        return;
    }

    showWelcomePage(*user);
}

void LoginWindow::attemptSignup() {
    const QString name = nameInput->text().trimmed();
    const QString username = signupUsernameInput->text().trimmed();
    const QString password = signupPasswordInput->text();
    const QString confirmation = confirmPasswordInput->text();

    if (name.isEmpty() || username.isEmpty() || password.isEmpty() || confirmation.isEmpty()) {
        signupFeedbackLabel->setStyleSheet("color: #b54747;");
        signupFeedbackLabel->setText("Complete all fields to create your account.");
        return;
    }
    if (password != confirmation) {
        signupFeedbackLabel->setStyleSheet("color: #b54747;");
        signupFeedbackLabel->setText("The passwords do not match.");
        confirmPasswordInput->clear();
        confirmPasswordInput->setFocus();
        return;
    }

    const std::string usernameText = username.toStdString();
    if (database.getUserByUsername(usernameText).has_value()) {
        signupFeedbackLabel->setStyleSheet("color: #b54747;");
        signupFeedbackLabel->setText("That username is already in use.");
        signupUsernameInput->setFocus();
        return;
    }

    const int userId = database.insertUser(
        usernameText,
        name.toStdString(),
        password.toStdString()
    );
    if (userId == -1) {
        signupFeedbackLabel->setStyleSheet("color: #b54747;");
        signupFeedbackLabel->setText("Could not create the account. Please try again.");
        return;
    }

    showLoginPage();
    feedbackLabel->setStyleSheet("color: #16856b;");
    feedbackLabel->setText("Account created. You can now sign in.");
}

void LoginWindow::showWelcomePage(const User& user) {
    QWidget* loginPage = takeCentralWidget();
    auto* page = new QWidget(this);
    page->setObjectName("welcomePage");
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(46, 48, 46, 42);
    layout->setSpacing(14);

    auto* brand = new QLabel("WELLNESS TRACKER", page);
    brand->setObjectName("brand");
    layout->addWidget(brand);

    auto* title = new QLabel(
        QString("Welcome, %1").arg(QString::fromStdString(user.getName())),
        page
    );
    title->setObjectName("title");
    title->setWordWrap(true);
    layout->addWidget(title);

    auto* subtitle = new QLabel("You are signed in.", page);
    subtitle->setObjectName("subtitle");
    layout->addWidget(subtitle);
    layout->addStretch();

    auto* signOutButton = new QPushButton("Sign out", page);
    layout->addWidget(signOutButton);
    connect(signOutButton, &QPushButton::clicked, this, [this] {
        QWidget* welcomePage = takeCentralWidget();
        usernameInput = nullptr;
        passwordInput = nullptr;
        feedbackLabel = nullptr;
        showStartPage();
        welcomePage->deleteLater();
    });

    setCentralWidget(page);
    loginPage->deleteLater();
}
