/**
 * @file main.cpp
 * @brief Entry point for the Inherit Memorizing Word System application
 *
 * @details This application is a comprehensive language learning tool designed to help users
 * memorize and practice vocabulary through an intelligent review system.
 *
 * Key Features:
 * - User authentication and management
 * - Vocabulary learning and review
 * - Adaptive learning algorithm
 * - Statistics and progress tracking
 *
 * Application Architecture:
 * - Models: Define core data structures (User, Word, ReviewSession)
 * - Repositories: Handle data persistence and database interactions
 * - Services: Implement business logic and application workflows
 * - UI: Provide user interface components
 *
 * Technology Stack:
 * - Language: C++
 * - GUI Framework: Qt 5.14.2
 * - Database: SQLite (via QSqlDatabase)
 *
 * @note The application uses a modern C++ design with dependency injection,
 * smart pointers, and service-based architecture.
 *
 * @author NJUPT
 * @version 1.0.0
 * @date 2024
 * @copyright All rights reserved
 */

#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include "ui/views/login_view.h"
#include "services/user_service.h"
#include "services/word_service.h"
#include "services/review_service.h"
#include "services/statistics_service.h"

// Project Structure:
/*
inherit_memorizing_word_system/
├── models/
│   ├── user.cpp/h
│   ├── word.cpp/h
│   └── review_session.cpp/h
├── repositories/
│   ├── base_repository.cpp/h
│   ├── user_repository.cpp/h
│   └── word_repository.cpp/h
├── services/
│   ├── user_service.cpp/h
│   ├── word_service.cpp/h
│   ├── review_service.cpp/h
│   └── statistics_service.cpp/h
├── ui/views/
│   ├── login_view.cpp/h
│   ├── vocabulary_view.cpp/h
│   ├── review_view.cpp/h
│   └── statistics_view.cpp/h
├── resources.qrc
└── main.cpp
*/

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Load and apply stylesheet
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        app.setStyleSheet(style);
        styleFile.close();
    }
    
    // Check available SQL drivers
    QStringList availableDrivers = QSqlDatabase::drivers();
    qDebug() << "Available SQL drivers:";
    for (const QString& driver : availableDrivers) {
        qDebug() << " -" << driver;
    }
    
    // Explicitly load SQLite driver
    const QString sqliteDriverName = "QSQLITE";
    if (!availableDrivers.contains(sqliteDriverName)) {
        QMessageBox::critical(nullptr, "Database Error", 
                            QString("SQLite driver (%1) is not available. Please check your Qt installation.")
                            .arg(sqliteDriverName));
        return 1;
    }
    
    // Initialize database connection
    QSqlDatabase db = QSqlDatabase::addDatabase(sqliteDriverName);
    db.setDatabaseName("word_system.db");
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", 
                            QString("Could not open database. Error: %1")
                            .arg(db.lastError().text()));
        return 1;
    }
    
    // Initialize database schema
    QSqlQuery query;
    
    // Users table
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                   "username TEXT PRIMARY KEY,"
                   "password TEXT,"
                   "total_score INTEGER DEFAULT 0,"
                   "days_streak INTEGER DEFAULT 0,"
                   "total_words_learned INTEGER DEFAULT 0,"
                   "last_checkin_date TEXT,"
                   "created_at TEXT"
                   ")")) {
        QMessageBox::critical(nullptr, "Database Error", 
                            QString("Could not create users table. Error: %1")
                            .arg(query.lastError().text()));
        return 1;
    }
    
    // Words table
    if (!query.exec("CREATE TABLE IF NOT EXISTS words ("
                   "english TEXT PRIMARY KEY,"
                   "part_of_speech TEXT,"
                   "chinese TEXT,"
                   "frequency INTEGER DEFAULT 0,"
                   "correct_count INTEGER DEFAULT 0,"
                   "total_attempts INTEGER DEFAULT 0"
                   ")")) {
        QMessageBox::critical(nullptr, "Database Error", 
                            QString("Could not create words table. Error: %1")
                            .arg(query.lastError().text()));
        return 1;
    }
    
    // Word definitions table
    if (!query.exec("CREATE TABLE IF NOT EXISTS word_definitions ("
                   "english TEXT,"
                   "definition_type TEXT,"
                   "content TEXT,"
                   "FOREIGN KEY(english) REFERENCES words(english) ON DELETE CASCADE"
                   ")")) {
        QMessageBox::critical(nullptr, "Database Error", 
                            QString("Could not create word_definitions table. Error: %1")
                            .arg(query.lastError().text()));
        return 1;
    }
    
    // Word categories table
    if (!query.exec("CREATE TABLE IF NOT EXISTS word_categories ("
                   "english TEXT,"
                   "category TEXT,"
                   "FOREIGN KEY(english) REFERENCES words(english) ON DELETE CASCADE"
                   ")")) {
        QMessageBox::critical(nullptr, "Database Error", 
                            QString("Could not create word_categories table. Error: %1")
                            .arg(query.lastError().text()));
        return 1;
    }
    
    // Initialize services
    auto userService = std::make_unique<UserService>();
    auto wordService = std::make_unique<WordService>();
    auto reviewService = std::make_unique<ReviewService>();
    auto statisticsService = std::make_unique<StatisticsService>();
    
    // Create and show login view
    LoginView loginView(userService.get());
    loginView.show();
    
    // Connect login success to main window creation
    QObject::connect(&loginView, &LoginView::loginSuccessful, 
        [&](const QString& username) {
            // Create and show main selection window
            auto mainWindow = new QMainWindow();
            auto centralWidget = new QWidget(mainWindow);
            auto layout = new QVBoxLayout(centralWidget);
            
            // Create buttons for different functionalities
            auto learnWordButton = new QPushButton("学习单词", mainWindow);
            auto reviewWordButton = new QPushButton("复习单词", mainWindow);
            auto statisticsButton = new QPushButton("学习统计", mainWindow);
            auto checkInButton = new QPushButton("每日签到", mainWindow);
            
            // Add buttons to layout
            layout->addWidget(learnWordButton);
            layout->addWidget(reviewWordButton);
            layout->addWidget(statisticsButton);
            layout->addWidget(checkInButton);
            
            mainWindow->setCentralWidget(centralWidget);
            mainWindow->setWindowTitle(QString("单词记忆系统 - %1").arg(username));
            mainWindow->resize(300, 400);
            
            // Connect button signals to respective views/services
            QObject::connect(learnWordButton, &QPushButton::clicked, 
                [wordService = wordService.get()]() {
                    // TODO: Open word learning view
                    QMessageBox::information(nullptr, "学习单词", "功能开发中");
                });
            
            QObject::connect(reviewWordButton, &QPushButton::clicked, 
                [reviewService = reviewService.get()]() {
                    // TODO: Open word review view
                    QMessageBox::information(nullptr, "复习单词", "功能开发中");
                });
            
            QObject::connect(statisticsButton, &QPushButton::clicked, 
                [statisticsService = statisticsService.get()]() {
                    // TODO: Open statistics view
                    QMessageBox::information(nullptr, "学习统计", "功能开发中");
                });
            
            QObject::connect(checkInButton, &QPushButton::clicked, 
                [userService = userService.get()]() {
                    try {
                        userService->checkIn();
                        
                        // Get the current user's stats
                        const auto& currentUser = userService->getCurrentUser();
                        const auto& stats = currentUser.getStats();
                        
                        // Create a detailed message
                        QString message = QString(
                            "签到成功！\n"
                            "连续签到天数：%1 天\n"
                            "获得积分：%2 分\n"
                            "总积分：%3 分"
                        ).arg(stats.daysStreak)
                         .arg(currentUser.calculateCheckInPoints())
                         .arg(stats.totalScore);
                        
                        QMessageBox::information(nullptr, "每日签到", message);
                    } catch (const std::exception& e) {
                        QMessageBox::warning(nullptr, "签到失败", e.what());
                    }
                });
            
            // Hide login view and show main window
            loginView.hide();
            mainWindow->show();
        });
    
    return app.exec();
}
