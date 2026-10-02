#include "MainWindow.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "DateUtils.h"
#include "PageLayout.h"

MainWindow::MainWindow(Database& database, QWidget* parent)
    : QMainWindow(parent),
      database(database),
      habitService(database),
      currentUser(std::nullopt),
      pages(new QStackedWidget(this)) {
    setWindowTitle("Wellness Tracker");
    resize(520, 620);
    setStyleSheet(
        "QLabel#title { font-size: 20px; font-weight: bold; }"
        "QLabel#feedback { color: #b54747; }"
        "QLabel#body { font-family: monospace; }"
        "QLineEdit, QPushButton { min-height: 28px; }"
    );

    // Added in the order of the Page enum.
    pages->addWidget(createStartPage());
    pages->addWidget(createLoginPage());
    pages->addWidget(createSignupPage());
    pages->addWidget(createHomePage());
    pages->addWidget(createHabitsPage());
    pages->addWidget(createProgressPage());
    pages->addWidget(createProfilePage());

    setCentralWidget(pages);
    showPage(StartPage);
}

void MainWindow::showPage(Page page) {
    pages->setCurrentIndex(page);
}

QWidget* MainWindow::createStartPage() {
    auto* page = new QWidget;
    auto* layout = startPage(page, "Wellness Tracker");

    layout->addWidget(new QLabel("Build healthy habits, one day at a time.", page));
    layout->addStretch();

    auto* login = new QPushButton("Log in", page);
    auto* signup = new QPushButton("Create an account", page);
    layout->addWidget(login);
    layout->addWidget(signup);

    connect(login, &QPushButton::clicked, this, [this] {
        showPage(LoginPage);
    });
    connect(signup, &QPushButton::clicked, this, [this] {
        showPage(SignupPage);
    });

    return page;
}

QWidget* MainWindow::createHomePage() {
    auto* page = new QWidget;
    auto* layout = startPage(page, "Home");

    homeGreeting = new QLabel(page);
    homeGreeting->setWordWrap(true);
    layout->addWidget(homeGreeting);
    layout->addStretch();

    auto* habits = new QPushButton("Log habits", page);
    auto* progress = new QPushButton("View progress", page);
    auto* profile = new QPushButton("View profile", page);
    auto* logout = new QPushButton("Log out", page);
    layout->addWidget(habits);
    layout->addWidget(progress);
    layout->addWidget(profile);
    layout->addWidget(logout);

    connect(habits, &QPushButton::clicked, this, [this] {
        habitFeedback->clear();
        showPage(HabitsPage);
    });
    connect(progress, &QPushButton::clicked, this, [this] {
        showPeriod("Today", {util::today()});
    });
    connect(profile, &QPushButton::clicked, this, [this] {
        showProfile();
    });
    connect(logout, &QPushButton::clicked, this, [this] {
        logOut();
    });

    return page;
}

void MainWindow::logOut() {
    currentUser = std::nullopt;
    loginUsername->clear();
    loginPassword->clear();
    loginFeedback->clear();
    showPage(StartPage);
}
