#include "mainwindow.h"
#include "views/LoginView.h"
#include "views/VocabularyView.h"
#include "views/ReviewView.h"
#include "views/StatisticsView.h"
#include <QMenuBar>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("单词记忆系统");
    resize(1024, 768);

    // 初始化核心组件
    currentUser = nullptr;
    vocabulary = new Vocabulary();
    // Review and Statistics will be initialized after user login
    review = nullptr;
    statistics = nullptr;

    // 创建主要容器
    stackedWidget = new QStackedWidget();
    setCentralWidget(stackedWidget);

    // 创建视图
    LoginView* loginView = new LoginView();
    connect(loginView, &LoginView::loginSuccess, this, &MainWindow::handleLoginSuccess);

    VocabularyView* vocabView = new VocabularyView(vocabulary);
    ReviewView* reviewView = new ReviewView(review);
    StatisticsView* statsView = new StatisticsView(statistics);

    // 添加视图到堆栈
    stackedWidget->addWidget(loginView);      // index 0
    stackedWidget->addWidget(vocabView);      // index 1
    stackedWidget->addWidget(reviewView);     // index 2
    stackedWidget->addWidget(statsView);      // index 3

    // 创建菜单栏
    createMenus();
}

void MainWindow::createMenus() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // 文件菜单
    QMenu* fileMenu = menuBar->addMenu("文件");
    QAction* importAction = fileMenu->addAction("导入词库");
    QAction* exportAction = fileMenu->addAction("导出词库");
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("退出");

    // 学习菜单
    QMenu* studyMenu = menuBar->addMenu("学习");
    QAction* reviewAction = studyMenu->addAction("开始复习");
    QAction* wrongWordsAction = studyMenu->addAction("错词本");

    // 设置菜单
    QMenu* settingsMenu = menuBar->addMenu("设置");
    QAction* profileAction = settingsMenu->addAction("个人信息");
    settingsMenu->addAction("偏好设置");  // Not connected yet
    settingsMenu->addSeparator();
    QAction* logoutAction = settingsMenu->addAction("注销");

    // Connect actions
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::handleLogout);
    connect(reviewAction, &QAction::triggered, this, &MainWindow::startWordReview);
    connect(wrongWordsAction, &QAction::triggered, this, &MainWindow::startWordReview);
    connect(importAction, &QAction::triggered, this, [this]() { vocabulary->importFromFile(""); });
    connect(exportAction, &QAction::triggered, this, [this]() { vocabulary->exportToFile(""); });
    connect(profileAction, &QAction::triggered, this, [this]() { stackedWidget->setCurrentIndex(3); });

    // Disable menu bar initially (will be enabled after login)
    menuBar->setEnabled(false);
}

void MainWindow::handleLoginSuccess(User* user) {
    currentUser = user;
    
    // Initialize Statistics and Review now that we have a user
    statistics = new Statistics(*currentUser);
    Study* study = new Study(*currentUser, *vocabulary);  // Create Study object for Review
    review = new Review(*statistics, *study);
    
    stackedWidget->setCurrentIndex(1);  // Switch to vocabulary view
    
    // Update menu state
    menuBar()->setEnabled(true);
    
    // 显示欢迎消息
    statusBar()->showMessage("欢迎回来, " + QString::fromStdString(user->getUsername()));
}

void MainWindow::startWordReview() {
    if (!currentUser) return;
    stackedWidget->setCurrentIndex(2);
}

void MainWindow::showStatistics() {
    if (!currentUser) return;
    stackedWidget->setCurrentIndex(3);
}

void MainWindow::handleLogout() {
    if (currentUser) {
        currentUser->logout();
        delete currentUser;
        currentUser = nullptr;
    }
    
    stackedWidget->setCurrentIndex(0);
    menuBar()->setEnabled(false);
    statusBar()->clearMessage();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (currentUser) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, 
            "确认退出", 
            "是否要保存今天的学习进度并退出？",
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            checkInAndExit();
        }
    }
    event->accept();
}

void MainWindow::checkInAndExit() {
    if (currentUser) {
        currentUser->checkIn();
        handleLogout();
    }
}

MainWindow::~MainWindow() {
    delete vocabulary;
    if (review) {
        delete review;
    }
    if (statistics) {
        delete statistics;
    }
    if (currentUser) {
        delete currentUser;
    }
}
