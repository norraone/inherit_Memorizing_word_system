#ifndef REVIEW_VIEW_H
#define REVIEW_VIEW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include "../../services/review_service.h"
#include "../../services/user_service.h"

class ReviewView : public QWidget {
    Q_OBJECT

private:
    ReviewService* reviewService;
    UserService* userService;
    
    // UI Components
    QLabel* wordLabel;
    QLabel* partOfSpeechLabel;
    QLineEdit* answerInput;
    QPushButton* checkButton;
    QPushButton* showAnswerButton;
    QPushButton* nextButton;
    
    // Statistics display
    QLabel* accuracyLabel;
    QLabel* timeLabel;
    QLabel* progressLabel;
    QTimer* timer;
    
    // Session state
    bool isAnswerShown;
    bool isAnswerChecked;
    
    void setupUi();
    void connectSignals();
    void updateStatistics();
    void showWord();
    void clearInput();
    void checkAnswer();
    void showAnswer();
    void nextWord();
    void endSession();
    void updateTimer();

private slots:
    void onCheckButtonClicked();
    void onShowAnswerButtonClicked();
    void onNextButtonClicked();
    void onAnswerInputReturnPressed();
    void onTimerTick();

signals:
    void sessionEnded(double accuracy, int totalWords, int correctWords);

public:
    explicit ReviewView(ReviewService* reviewService, 
                       UserService* userService,
                       QWidget* parent = nullptr);
    ~ReviewView() override;
    
    void startNewSession(int wordCount = 10);
};

#endif // REVIEW_VIEW_H
