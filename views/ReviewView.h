#ifndef REVIEWVIEW_H
#define REVIEWVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include "../review.h"

class ReviewView : public QWidget {
    Q_OBJECT

private:
    Review* review;
    QLabel* wordLabel;
    QLineEdit* answerInput;
    QPushButton* checkButton;
    QPushButton* nextButton;
    QLabel* scoreLabel;
    QLabel* streakLabel;
    QStackedWidget* modeStack;

    void setupUI();
    void connectSignals();
    void updateStats();
    void showResult(bool correct);

private slots:
    void handleAnswer();
    void handleNext();
    void switchMode(int mode);

public:
    explicit ReviewView(Review* rev, QWidget* parent = nullptr);
};

#endif // REVIEWVIEW_H
