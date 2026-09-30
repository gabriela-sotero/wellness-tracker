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
      feedbackLabel(nullptr) {
    setWindowTitle("Wellness Tracker");
    setMinimumSize(420, 520);
    resize(460, 560);
    setStyleSheet(R"(
        QMainWindow, QWidget#loginPage, QWidget#welcomePage {
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

    setCentralWidget(createLoginPage());
}

QWidget* LoginWindow::createLoginPage() {
    auto* page = new QWidget(this);
    page->setObjectName("loginPage");
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(46, 48, 46, 42);
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

    auto* formPanel = new QWidget(page);
    formPanel->setMaximumWidth(360);
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

    return page;
}

void LoginWindow::attemptLogin() {
    const QString username = usernameInput->text().trimmed();
    const QString password = passwordInput->text();

    if (username.isEmpty() || password.isEmpty()) {
        feedbackLabel->setText("Enter your username and password.");
        return;
    }

    const auto user = database.authenticate(
        username.toStdString(),
        password.toStdString()
    );
    if (!user.has_value()) {
        feedbackLabel->setText("Username or password is incorrect.");
        passwordInput->clear();
        passwordInput->setFocus();
        return;
    }

    showWelcomePage(*user);
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
        setCentralWidget(createLoginPage());
        welcomePage->deleteLater();
    });

    setCentralWidget(page);
    loginPage->deleteLater();
}
