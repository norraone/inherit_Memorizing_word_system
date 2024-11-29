#include "ReviewView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QMessageBox>

ReviewView::ReviewView(Review* rev, QWidget* parent)
    : QWidget(parent), review(rev) {
    setupUI();
    connectSignals();
}

void ReviewView::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // 学习模式选择
    auto* modeLayout = new QHBoxLayout();
    auto* modeGroup = new QButtonGroup(this);
    
    auto* normalMode = new QRadioButton("普通模式", this);
    auto* challengeMode = new QRadioButton("挑战模式", this);
    auto* reviewMode = new QRadioButton("错词复习", this);
    
    modeGroup->addButton(normalMode, 0);
    modeGroup->addButton(challengeMode, 1);
    modeGroup->addButton(reviewMode, 2);
    
    modeLayout->addWidget(normalMode);
    modeLayout->addWidget(challengeMode);
    modeLayout->addWidget(reviewMode);
    modeLayout->addStretch();
    
    mainLayout->addLayout(modeLayout);

    // 统计信息
    auto* statsLayout = new QHBoxLayout();
    scoreLabel = new QLabel("得分: 0", this);
    streakLabel = new QLabel("连续正确: 0", this);
    statsLayout->addWidget(scoreLabel);
    statsLayout->addWidget(streakLabel);
    statsLayout->addStretch();
    mainLayout->addLayout(statsLayout);

    // 单词显示区域
    wordLabel = new QLabel(this);
    wordLabel->setAlignment(Qt::AlignCenter);
    wordLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin: 20px 0;");
    mainLayout->addWidget(wordLabel);

    // 答案输入
    answerInput = new QLineEdit(this);
    answerInput->setPlaceholderText("请输入答案...");
    mainLayout->addWidget(answerInput);

    // 按钮区域
    auto* buttonLayout = new QHBoxLayout();
    checkButton = new QPushButton("检查", this);
    nextButton = new QPushButton("下一个", this);
    nextButton->setEnabled(false);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(checkButton);
    buttonLayout->addWidget(nextButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);

    // 添加弹性空间
    mainLayout->addStretch();

    // 设置样式
    setStyleSheet(R"(
        QWidget {
            background-color: white;
        }
        QRadioButton {
            font-size: 14px;
            padding: 5px;
        }
        QLabel {
            color: #333;
        }
        QLineEdit {
            padding: 10px;
            border: 1px solid #ddd;
            border-radius: 4px;
            font-size: 16px;
        }
        QPushButton {
            padding: 8px 20px;
            border: none;
            border-radius: 4px;
            font-size: 14px;
            min-width: 100px;
        }
        QPushButton#checkButton {
            background-color: #4CAF50;
            color: white;
        }
        QPushButton#checkButton:hover {
            background-color: #45a049;
        }
        QPushButton#nextButton {
            background-color: #2196F3;
            color: white;
        }
        QPushButton#nextButton:hover {
            background-color: #1976D2;
        }
        QPushButton:disabled {
            background-color: #cccccc;
        }
    )");
}

void ReviewView::connectSignals() {
    connect(checkButton, &QPushButton::clicked, this, &ReviewView::handleAnswer);
    connect(nextButton, &QPushButton::clicked, this, &ReviewView::handleNext);
    connect(answerInput, &QLineEdit::returnPressed, this, &ReviewView::handleAnswer);
}

void ReviewView::handleAnswer() {
    QString answer = answerInput->text().trimmed();
    if (answer.isEmpty()) return;

    bool correct = review->checkAnswer(answer.toStdString());
    showResult(correct);
    
    checkButton->setEnabled(false);
    nextButton->setEnabled(true);
    answerInput->setEnabled(false);
    
    updateStats();
}

void ReviewView::handleNext() {
    checkButton->setEnabled(true);
    nextButton->setEnabled(false);
    answerInput->setEnabled(true);
    answerInput->clear();
    answerInput->setFocus();
    
    if (review->hasNextWord()) {
        wordLabel->setText(QString::fromStdString(review->getNextWord()));
    } else {
        QMessageBox::information(this, "完成", "本轮复习完成！");
        // TODO: 显示本轮复习统计
    }
}

void ReviewView::updateStats() {
    scoreLabel->setText(QString("得分: %1").arg(review->getScore()));
    streakLabel->setText(QString("连续正确: %1").arg(review->getStreak()));
}

void ReviewView::showResult(bool correct) {
    QString style = correct ? 
        "color: #4CAF50; font-weight: bold;" : 
        "color: #f44336; font-weight: bold;";
    
    wordLabel->setStyleSheet(wordLabel->styleSheet() + style);
    
    if (!correct) {
        // 显示正确答案
        QString correctAnswer = QString::fromStdString(review->getCurrentWord().getChinese());
        QMessageBox::information(this, "错误", 
            QString("正确答案是: %1").arg(correctAnswer));
    }
}
