#include "review_view.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFont>

ReviewView::ReviewView(ReviewService* rService, UserService* uService, QWidget* parent)
    : QWidget(parent), reviewService(rService), userService(uService),
      isAnswerShown(false), isAnswerChecked(false) {
    setupUi();
    connectSignals();
    
    timer = new QTimer(this);
    timer->setInterval(1000);  // Update every second
    connect(timer, &QTimer::timeout, this, &ReviewView::onTimerTick);
}

ReviewView::~ReviewView() {
    if (timer->isActive()) {
        timer->stop();
    }
}

void ReviewView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    
    // Word display area
    auto* wordArea = new QWidget(this);
    auto* wordLayout = new QVBoxLayout(wordArea);
    
    wordLabel = new QLabel(wordArea);
    wordLabel->setAlignment(Qt::AlignCenter);
    QFont wordFont = wordLabel->font();
    wordFont.setPointSize(24);
    wordLabel->setFont(wordFont);
    
    partOfSpeechLabel = new QLabel(wordArea);
    partOfSpeechLabel->setAlignment(Qt::AlignCenter);
    
    wordLayout->addWidget(wordLabel);
    wordLayout->addWidget(partOfSpeechLabel);
    
    // Answer input area
    auto* inputArea = new QWidget(this);
    auto* inputLayout = new QHBoxLayout(inputArea);
    
    answerInput = new QLineEdit(inputArea);
    answerInput->setPlaceholderText("输入中文释义...");
    checkButton = new QPushButton("检查", inputArea);
    showAnswerButton = new QPushButton("显示答案", inputArea);
    
    inputLayout->addWidget(answerInput);
    inputLayout->addWidget(checkButton);
    inputLayout->addWidget(showAnswerButton);
    
    // Next button
    nextButton = new QPushButton("下一个", this);
    nextButton->setEnabled(false);
    
    // Statistics area
    auto* statsArea = new QWidget(this);
    auto* statsLayout = new QHBoxLayout(statsArea);
    
    accuracyLabel = new QLabel("正确率: 0%", statsArea);
    timeLabel = new QLabel("时间: 00:00", statsArea);
    progressLabel = new QLabel("进度: 0/0", statsArea);
    
    statsLayout->addWidget(accuracyLabel);
    statsLayout->addWidget(timeLabel);
    statsLayout->addWidget(progressLabel);
    
    // Add all components to main layout
    mainLayout->addWidget(wordArea);
    mainLayout->addWidget(inputArea);
    mainLayout->addWidget(nextButton);
    mainLayout->addWidget(statsArea);
}

void ReviewView::connectSignals() {
    connect(checkButton, &QPushButton::clicked,
            this, &ReviewView::onCheckButtonClicked);
    connect(showAnswerButton, &QPushButton::clicked,
            this, &ReviewView::onShowAnswerButtonClicked);
    connect(nextButton, &QPushButton::clicked,
            this, &ReviewView::onNextButtonClicked);
    connect(answerInput, &QLineEdit::returnPressed,
            this, &ReviewView::onAnswerInputReturnPressed);
}

void ReviewView::startNewSession(int wordCount) {
    try {
        reviewService->startNewSession(userService->getCurrentUser().getUsername(), wordCount);
        timer->start();
        showWord();
        updateStatistics();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "错误",
            QString("无法开始复习会话: %1").arg(e.what()));
    }
}

void ReviewView::showWord() {
    if (!reviewService->hasActiveSession()) return;
    
    const Word& word = reviewService->getCurrentWord();
    wordLabel->setText(QString::fromStdString(word.getEnglish()));
    partOfSpeechLabel->setText(QString::fromStdString(word.getPartOfSpeech()));
    
    isAnswerShown = false;
    isAnswerChecked = false;
    answerInput->clear();
    answerInput->setEnabled(true);
    checkButton->setEnabled(true);
    showAnswerButton->setEnabled(true);
    nextButton->setEnabled(false);
}

void ReviewView::checkAnswer() {
    if (!reviewService->hasActiveSession() || isAnswerChecked) return;
    
    const Word& word = reviewService->getCurrentWord();
    QString userAnswer = answerInput->text().trimmed();
    QString correctAnswer = QString::fromStdString(word.getChinese());
    
    bool isCorrect = (userAnswer == correctAnswer);
    reviewService->recordAttempt(isCorrect);
    
    if (isCorrect) {
        answerInput->setStyleSheet("QLineEdit { background-color: #d4edda; }");
        userService->addScore(10);  // Award points for correct answer
    } else {
        answerInput->setStyleSheet("QLineEdit { background-color: #f8d7da; }");
        showAnswer();
    }
    
    isAnswerChecked = true;
    answerInput->setEnabled(false);
    checkButton->setEnabled(false);
    nextButton->setEnabled(true);
    
    updateStatistics();
}

void ReviewView::showAnswer() {
    if (!reviewService->hasActiveSession() || isAnswerShown) return;
    
    const Word& word = reviewService->getCurrentWord();
    answerInput->setText(QString::fromStdString(word.getChinese()));
    answerInput->setStyleSheet("QLineEdit { background-color: #fff3cd; }");
    isAnswerShown = true;
    
    checkButton->setEnabled(false);
    showAnswerButton->setEnabled(false);
    nextButton->setEnabled(true);
}

void ReviewView::nextWord() {
    if (!reviewService->hasActiveSession()) return;
    
    if (reviewService->hasNextWord()) {
        showWord();
    } else {
        endSession();
    }
    
    answerInput->setStyleSheet("");
    updateStatistics();
}

void ReviewView::endSession() {
    if (!reviewService->hasActiveSession()) return;
    
    timer->stop();
    double accuracy = reviewService->getCurrentAccuracy();
    int totalWords = reviewService->getTotalCount();
    int correctWords = reviewService->getCorrectCount();
    
    reviewService->endSession();
    
    QString message = QString(
        "复习会话结束!\n"
        "正确率: %.1f%%\n"
        "总单词数: %1\n"
        "正确数: %2\n"
        "用时: %3")
        .arg(accuracy * 100)
        .arg(totalWords)
        .arg(correctWords)
        .arg(timeLabel->text().mid(3));  // Remove "时间: " prefix
    
    QMessageBox::information(this, "会话结果", message);
    emit sessionEnded(accuracy, totalWords, correctWords);
}

void ReviewView::updateStatistics() {
    if (!reviewService->hasActiveSession()) return;
    
    accuracyLabel->setText(QString("正确率: %.1f%%")
        .arg(reviewService->getCurrentAccuracy() * 100));
    
    progressLabel->setText(QString("进度: %1/%2")
        .arg(reviewService->getTotalCount())
        .arg(reviewService->getReviewedItems().size()));
}

void ReviewView::updateTimer() {
    if (!reviewService->hasActiveSession()) return;
    
    auto elapsed = reviewService->getSessionTime();
    int minutes = static_cast<int>(elapsed.count()) / 60;
    int seconds = static_cast<int>(elapsed.count()) % 60;
    
    timeLabel->setText(QString("时间: %1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));
}

void ReviewView::onCheckButtonClicked() {
    checkAnswer();
}

void ReviewView::onShowAnswerButtonClicked() {
    showAnswer();
}

void ReviewView::onNextButtonClicked() {
    nextWord();
}

void ReviewView::onAnswerInputReturnPressed() {
    if (!isAnswerChecked && checkButton->isEnabled()) {
        checkAnswer();
    } else if (isAnswerChecked && nextButton->isEnabled()) {
        nextWord();
    }
}

void ReviewView::onTimerTick() {
    updateTimer();
}
