#include "MainWindow.h"

#include <QColor>
#include <QDate>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>

#include "DateUtils.h"
#include "Icons.h"
#include "PageLayout.h"
#include "PathView.h"

MainWindow::MainWindow(Database& database, QWidget* parent)
    : QMainWindow(parent),
      database(database),
      habitService(database),
      currentUser(std::nullopt),
      pages(new QStackedWidget(this)) {
    // MainWindow borrows persistence, composes the application service, and owns
    // each page through Qt's parent-child object model.
    setWindowTitle("Wellness Tracker");
    resize(480, 820);
    setMinimumSize(360, 560);

    // Palette: primary #007c68 with its pressed "edge" #005c4d, soft tints for
    // active states, gold and orange only for streak and XP accents.
    setStyleSheet(R"qss(
        QMainWindow, QWidget#page {
            background-color: #ffffff;
        }
        QScrollArea {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            width: 10px;
            margin: 2px;
            background: transparent;
        }
        QScrollBar::handle:vertical {
            min-height: 32px;
            border-radius: 3px;
            background: #cfdcd8;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
        }
        QToolTip {
            background-color: #183b35;
            color: #ffffff;
            border: none;
            padding: 6px 10px;
        }

        QLabel {
            color: #183b35;
            font-size: 14px;
        }
        QLabel#title {
            font-size: 30px;
            font-weight: 800;
        }
        QLabel#subtitle {
            color: #6b8780;
            font-size: 16px;
        }
        QLabel#greeting {
            font-size: 20px;
            font-weight: 800;
        }
        QLabel#heading {
            font-size: 17px;
            font-weight: 800;
        }
        QLabel#cardTitle {
            font-size: 16px;
            font-weight: 800;
        }
        QLabel#muted {
            color: #7a8f89;
        }
        QLabel#value {
            font-weight: 700;
        }
        QLabel#total {
            font-size: 16px;
            font-weight: 800;
            color: #007c68;
        }
        QLabel#name {
            font-size: 20px;
            font-weight: 800;
        }
        QLabel#feedback {
            color: #d64545;
            font-size: 13px;
            font-weight: 700;
        }
        QLabel#streakValue {
            color: #ff9600;
            font-size: 17px;
            font-weight: 800;
        }
        QLabel#xpValue {
            color: #d9a400;
            font-size: 17px;
            font-weight: 800;
        }

        QWidget#topBar {
            background-color: #ffffff;
            border-bottom: 2px solid #e3eae8;
        }
        QWidget#navBar {
            background-color: #ffffff;
            border-top: 2px solid #e3eae8;
        }
        QWidget#card {
            background-color: #ffffff;
            border: 2px solid #e3eae8;
            border-radius: 16px;
        }

        QLineEdit {
            min-height: 40px;
            padding: 0 14px;
            border: 2px solid #e3eae8;
            border-radius: 12px;
            background-color: #f5f8f7;
            color: #183b35;
            font-size: 14px;
            selection-background-color: #007c68;
            selection-color: #ffffff;
        }
        QLineEdit:focus {
            border: 2px solid #007c68;
            background-color: #ffffff;
        }

        QPushButton {
            min-height: 40px;
            padding: 0 18px;
            border: none;
            border-bottom: 4px solid #005c4d;
            border-radius: 14px;
            background-color: #007c68;
            color: #ffffff;
            font-size: 14px;
            font-weight: 800;
        }
        QPushButton:hover {
            background-color: #0a8c77;
        }
        QPushButton:pressed {
            border-bottom: 1px solid #005c4d;
            padding-top: 3px;
        }
        QPushButton#secondary {
            background-color: #ffffff;
            color: #007c68;
            border: 2px solid #e3eae8;
            border-bottom: 4px solid #e3eae8;
        }
        QPushButton#secondary:hover {
            background-color: #f2f8f6;
        }
        QPushButton#secondary:pressed {
            border-bottom: 2px solid #e3eae8;
            padding-top: 2px;
        }
        QPushButton#danger {
            background-color: #ffffff;
            color: #d64545;
            border: 2px solid #f1d0d0;
            border-bottom: 4px solid #f1d0d0;
        }
        QPushButton#danger:hover {
            background-color: #fdf2f2;
        }
        QPushButton#danger:pressed {
            border-bottom: 2px solid #f1d0d0;
            padding-top: 2px;
        }
        QPushButton#tab {
            min-height: 34px;
            padding: 0 4px;
            border: 2px solid #e3eae8;
            border-radius: 12px;
            background-color: #ffffff;
            color: #6b8780;
            font-size: 13px;
        }
        QPushButton#tab:hover {
            background-color: #f2f8f6;
        }
        QPushButton#tab[active="true"] {
            border: 2px solid #007c68;
            background-color: #e0f2ee;
            color: #007c68;
        }
        QPushButton#nav {
            min-height: 46px;
            padding: 0;
            border: 2px solid transparent;
            border-radius: 14px;
            background-color: transparent;
        }
        QPushButton#nav:hover {
            background-color: #f2f8f6;
        }
        QPushButton#nav[active="true"] {
            border: 2px solid #8fcfc3;
            background-color: #e0f2ee;
        }

        QProgressBar {
            min-height: 24px;
            border: none;
            border-radius: 12px;
            background-color: #e9f0ed;
            color: #183b35;
            font-size: 12px;
            font-weight: 700;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #7fd0c1;
            border-radius: 12px;
        }
    )qss");

    // Added in the order of the Page enum.
    pages->addWidget(createStartPage());
    pages->addWidget(createLoginPage());
    pages->addWidget(createSignupPage());
    pages->addWidget(createHomePage());
    pages->addWidget(createHabitsPage());
    pages->addWidget(createProgressPage());
    pages->addWidget(createProfilePage());

    // The page stack fills the window; the nav bar sits under it on signed-in pages.
    auto* shell = new QWidget(this);
    auto* shellLayout = new QVBoxLayout(shell);
    shellLayout->setContentsMargins(0, 0, 0, 0);
    shellLayout->setSpacing(0);
    shellLayout->addWidget(pages, 1);
    navBar = createNavBar();
    shellLayout->addWidget(navBar);

    setCentralWidget(shell);
    showPage(StartPage);
}

void MainWindow::showPage(Page page) {
    // Centralize stack navigation so page selection stays consistent.
    pages->setCurrentIndex(page);

    const bool signedIn = page >= HomePage;
    navBar->setVisible(signedIn);
    if (signedIn) {
        updateNav(page);
    }
    if (page == HomePage) {
        refreshHome();
    }
    if (page == HabitsPage) {
        for (QuestCard* quest : {&waterQuest, &mealQuest, &exerciseQuest, &sleepQuest}) {
            quest->feedback->clear();
            quest->feedback->hide();
        }
        refreshHabits();
    }
}

QWidget* MainWindow::createNavBar() {
    // Four icon-only buttons, centered as a group so they stay close together on
    // wide windows. Icons and highlight are applied by updateNav.
    auto* bar = new QWidget(this);
    bar->setObjectName("navBar");
    bar->setAttribute(Qt::WA_StyledBackground, true);

    auto* layout = new QHBoxLayout(bar);
    layout->setContentsMargins(16, 8, 16, 10);
    layout->setSpacing(8);

    const char* names[] = {"Home", "Log habits", "Profile", "Sign out"};
    layout->addStretch();
    for (const char* name : names) {
        auto* button = new QPushButton(bar);
        button->setObjectName("nav");
        button->setToolTip(name);
        button->setAccessibleName(name);
        button->setCursor(Qt::PointingHandCursor);
        button->setIconSize(QSize(28, 28));
        button->setMinimumWidth(64);
        button->setMaximumWidth(120);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        layout->addWidget(button);
        navButtons.push_back(button);
    }
    layout->addStretch();

    connect(navButtons[0], &QPushButton::clicked, this, [this] {
        showPage(HomePage);
    });
    connect(navButtons[1], &QPushButton::clicked, this, [this] {
        showPage(HabitsPage);
    });
    connect(navButtons[2], &QPushButton::clicked, this, [this] {
        showProfile();
    });
    connect(navButtons[3], &QPushButton::clicked, this, [this] {
        logOut();
    });

    return bar;
}

void MainWindow::updateNav(Page page) {
    // The progress screen is reached from home, so home stays highlighted there.
    int active = -1;
    switch (page) {
        case HomePage:
        case ProgressPage:
            active = 0;
            break;
        case HabitsPage:
            active = 1;
            break;
        case ProfilePage:
            active = 2;
            break;
        default:
            break;
    }

    const char* bodies[] = {icons::home, icons::log, icons::profile, icons::logout};
    for (int i = 0; i < static_cast<int>(navButtons.size()); ++i) {
        const bool on = i == active;
        setActive(navButtons[i], on);
        navButtons[i]->setIcon(QIcon(
            icons::svgPixmap(bodies[i], QColor(on ? "#007c68" : "#8fa39d"), 28)
        ));
    }
}

QWidget* MainWindow::createStartPage() {
    // Build the public landing screen and connect its buttons to stack navigation.
    auto* page = new QWidget;
    auto* layout = startPage(page, "Wellness Tracker");

    // startPage put the title first; center it and place the emblem above it.
    if (auto* title = qobject_cast<QLabel*>(layout->itemAt(0)->widget())) {
        title->setAlignment(Qt::AlignCenter);
    }
    auto* emblem = new QLabel(page);
    emblem->setAlignment(Qt::AlignCenter);
    emblem->setPixmap(icons::svgPixmap(
        icons::emblem, QColor("#ffffff"), 96, 100, QString("0 0 96 100")
    ));
    layout->insertWidget(0, emblem);

    auto* subtitle = new QLabel("Build healthy habits, one day at a time.", page);
    subtitle->setObjectName("subtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    layout->addWidget(subtitle);
    layout->addSpacing(24);

    auto* signup = new QPushButton("Get started", page);
    auto* login = new QPushButton("I already have an account", page);
    login->setObjectName("secondary");
    layout->addWidget(signup);
    layout->addWidget(login);

    connect(login, &QPushButton::clicked, this, [this] {
        showPage(LoginPage);
    });
    connect(signup, &QPushButton::clicked, this, [this] {
        showPage(SignupPage);
    });

    return page;
}

QWidget* MainWindow::createHomePage() {
    // Header with the user's stats on top; below it the day path, which scrolls.
    auto* page = new QWidget;
    page->setObjectName("page");
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* header = new QWidget(page);
    header->setObjectName("topBar");
    header->setAttribute(Qt::WA_StyledBackground, true);
    auto* headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(16, 10, 16, 10);
    headerLayout->setSpacing(7);

    auto* topRow = new QHBoxLayout;
    topRow->setSpacing(8);

    auto* who = new QVBoxLayout;
    who->setSpacing(0);
    homeGreeting = new QLabel(header);
    homeGreeting->setObjectName("greeting");
    homeGreeting->setWordWrap(true);
    homeLevel = new QLabel(header);
    homeLevel->setObjectName("muted");
    who->addWidget(homeGreeting);
    who->addWidget(homeLevel);
    topRow->addLayout(who, 1);

    // XP stays on the greeting row; habit streaks form a compact strip below.
    auto addChip = [&](QHBoxLayout* chips, const char* icon, const QColor& color,
                       const char* valueName, const QString& tooltip) {
        auto* chip = new QWidget(header);
        chip->setToolTip(tooltip);
        chip->setAccessibleName(tooltip);
        auto* chipLayout = new QHBoxLayout(chip);
        chipLayout->setContentsMargins(0, 0, 0, 0);
        chipLayout->setSpacing(3);

        auto* image = new QLabel(chip);
        image->setPixmap(icons::svgPixmap(icon, color, 18));
        image->setToolTip(tooltip);
        image->setAccessibleName(tooltip);
        auto* value = new QLabel("0", chip);
        value->setObjectName(valueName);
        value->setToolTip(tooltip);
        value->setAccessibleName(tooltip);
        chipLayout->addWidget(image);
        chipLayout->addWidget(value);
        chips->addWidget(chip);
        return value;
    };
    homeXp = addChip(topRow, icons::bolt, QColor("#ffc800"), "xpValue", "Total XP");
    headerLayout->addLayout(topRow);

    auto* streakRow = new QHBoxLayout;
    streakRow->setContentsMargins(0, 0, 0, 0);
    streakRow->setSpacing(18);
    const struct { const char* icon; const char* name; QColor color; } streaks[] = {
        {icons::drop, "Water streak", QColor("#1cb0f6")},
        {icons::meal, "Healthy meals streak", QColor("#ff9600")},
        {icons::dumbbell, "Exercise streak", QColor("#ff4b4b")},
        {icons::moon, "Sleep streak", QColor("#a560f0")}
    };
    for (int i = 0; i < 4; ++i) {
        homeHabitStreaks.push_back(addChip(
            streakRow, streaks[i].icon, streaks[i].color, "streakValue",
            QString("%1 · consecutive days").arg(streaks[i].name)
        ));
    }
    streakRow->addStretch();
    headerLayout->addLayout(streakRow);
    layout->addWidget(header);

    homePath = new PathView;
    homePath->onDayClicked = [this](const QDate& date) {
        // Today uses the service's own date string; other days are formatted
        // the same way the progress screen displays them (yyyy-MM-dd).
        if (date == QDate::currentDate()) {
            showPeriod("Today", {util::today()});
        } else {
            showPeriod("Day", {date.toString("yyyy-MM-dd").toStdString()});
        }
    };
    homePath->goalsMetOn = [this](const QDate& date) {
        return goalsMetOn(date);
    };

    homeScroll = new QScrollArea(page);
    homeScroll->setWidgetResizable(true);
    homeScroll->setFrameShape(QFrame::NoFrame);
    homeScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    homeScroll->setWidget(homePath);
    layout->addWidget(homeScroll, 1);

    connect(homeScroll->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        extendPathAtEdges(value);
    });

    return page;
}

void MainWindow::refreshHome() {
    // Stats come from the service; the path restarts at today so a returning
    // user always lands on the current day.
    if (!currentUser.has_value()) {
        return;
    }

    const int userId = currentUser->getId();
    const ProfileSummary profile = habitService.profileSummary(userId);
    const PeriodSummary today = habitService.periodSummary(userId, {util::today()});

    homeLevel->setText(QString("Level %1").arg(profile.levelProgress.level));
    const int habitStreaks[] = {
        profile.waterStreak,
        profile.healthyMealsStreak,
        profile.exerciseStreak,
        profile.sleepStreak
    };
    for (std::size_t i = 0; i < homeHabitStreaks.size(); ++i) {
        homeHabitStreaks[i]->setText(QString::number(habitStreaks[i]));
    }
    homeXp->setText(QString::number(profile.totalXp));

    // History stops at the day the account was created; without a usable date
    // it falls back to the last two months.
    QDate earliest;
    if (const auto createdAt = database.accountCreatedAt(userId)) {
        earliest = QDate::fromString(QString::fromStdString(*createdAt).left(10), "yyyy-MM-dd");
    }
    if (!earliest.isValid()) {
        earliest = QDate::currentDate().addDays(-60);
    }

    homePath->reset(earliest);
    homePath->setTodayGoals(today.overallGoalsMet);

    // Land with today near the top so the first locked days show right below
    // it and the history is one scroll up. The second call covers the first
    // layout pass, when the scroll range may not be final yet.
    auto scrollToToday = [this] {
        QScrollBar* bar = homeScroll->verticalScrollBar();
        bar->setValue(std::min(bar->maximum(), std::max(0, homePath->todayY() - 170)));
    };
    scrollToToday();
    QTimer::singleShot(0, this, scrollToToday);
}

void MainWindow::extendPathAtEdges(int scrollValue) {
    if (extendingPath) {
        return;
    }
    QScrollBar* bar = homeScroll->verticalScrollBar();

    if (scrollValue <= 80 && homePath->canExtendPast()) {
        // Reaching the top loads two more weeks of history. They are added above
        // the current days, so the distance from the bottom is restored afterwards
        // to keep what the user is looking at where it was.
        extendingPath = true;
        const int distanceFromBottom = bar->maximum() - bar->value();
        homePath->extendPast(14);
        bar->setValue(bar->maximum() - distanceFromBottom);

        QTimer::singleShot(0, this, [this, distanceFromBottom] {
            QScrollBar* settled = homeScroll->verticalScrollBar();
            settled->setValue(settled->maximum() - distanceFromBottom);
            extendingPath = false;
        });
    } else if (bar->maximum() > 0 && scrollValue >= bar->maximum() - 80
               && homePath->canExtendFuture()) {
        // More locked days below leave the scroll position untouched.
        homePath->extendFuture(14);
    }
}

int MainWindow::goalsMetOn(const QDate& date) {
    // One service call per day; PathView caches the answer.
    if (!currentUser.has_value()) {
        return -1;
    }
    const PeriodSummary summary = habitService.periodSummary(
        currentUser->getId(), {date.toString("yyyy-MM-dd").toStdString()}
    );
    return summary.days == 0 ? -1 : static_cast<int>(summary.overallGoalsMet);
}

void MainWindow::logOut() {
    // Clear session-specific form state before returning to the welcome page.
    currentUser = std::nullopt;
    loginUsername->clear();
    loginPassword->clear();
    loginFeedback->clear();
    showPage(StartPage);
}
