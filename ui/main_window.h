#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "../services/word_service.h"
#include "../services/user_service.h"
#include "views/login_view.h"
#include "views/vocabulary_view.h"
#include "views/review_view.h"
#include "views/statistics_view.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // Services
    std::unique_ptr<WordService> wordService;
    std::unique_ptr<UserService> userService;

    // UI Components
    QStackedWidget* stackedWidget;
    LoginView* loginView;
    VocabularyView* vocabularyView;
    ReviewView* reviewView;
    StatisticsView* statisticsView;

    void setupUi();
    void createMenus();
    void connectSignals();

private slots:
    void handleLoginSuccess(const QString& username);
    void handleLogout();
    void navigateToView(int viewIndex);

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
};

#endif // MAIN_WINDOW_H
