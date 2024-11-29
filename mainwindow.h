#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include "user.h"
#include "vocabulary.h"
#include "review.h"
#include "statistics.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // Core components
    QStackedWidget* stackedWidget;
    User* currentUser;
    Vocabulary* vocabulary;
    Review* review;
    Statistics* statistics;
    
    // UI Pages
    QWidget* createLoginPage();
    QWidget* createRegisterPage();
    QWidget* createMainMenuPage();
    QWidget* createVocabularyPage();
    QWidget* createReviewPage();
    QWidget* createStatisticsPage();
    
    // UI Elements
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QLabel* scoreLabel;
    QLabel* streakLabel;
    
private slots:
    void handleLogin();
    void handleRegister();
    void startWordReview();
    void showStatistics();
    void manageVocabulary();
    void handleLogout();
    void checkInAndExit();

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent* event) override;
};

#endif // MAINWINDOW_H
