#include "StatisticsView.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

StatisticsView::StatisticsView(Statistics* stats, QWidget* parent)
    : QWidget(parent), statistics(stats)
{
    setupUI();
    connectSignals();
    updateStats();
}

void StatisticsView::setupUI()
{
    auto layout = new QVBoxLayout(this);
    
    // 创建时间范围选择器
    timeRangeSelector = new QComboBox(this);
    timeRangeSelector->addItem("今日");
    timeRangeSelector->addItem("本周");
    timeRangeSelector->addItem("本月");
    timeRangeSelector->addItem("全部");
    
    // 创建统计信息标签
    accuracyLabel = new QLabel(this);
    totalWordsLabel = new QLabel(this);
    wrongWordsLabel = new QLabel(this);
    
    // 创建导出按钮
    auto exportButton = new QPushButton("导出统计", this);
    
    // 布局
    auto statsGrid = new QGridLayout;
    statsGrid->addWidget(new QLabel("准确率:", this), 0, 0);
    statsGrid->addWidget(accuracyLabel, 0, 1);
    statsGrid->addWidget(new QLabel("总单词数:", this), 1, 0);
    statsGrid->addWidget(totalWordsLabel, 1, 1);
    statsGrid->addWidget(new QLabel("错误单词数:", this), 2, 0);
    statsGrid->addWidget(wrongWordsLabel, 2, 1);
    
    layout->addWidget(timeRangeSelector);
    layout->addLayout(statsGrid);
    layout->addWidget(exportButton);
    layout->addStretch();
}

void StatisticsView::connectSignals()
{
    connect(timeRangeSelector, &QComboBox::currentTextChanged,
            this, &StatisticsView::handleTimeRangeChange);
    connect(exportButton, &QPushButton::clicked, this, &StatisticsView::exportStats);
}

void StatisticsView::updateStats()
{
    // 获取统计数据
    double accuracy = statistics->getOverallAccuracy() * 100;
    int wrongWordsCount = statistics->getWrongWordsCount();
    auto todayStats = statistics->getTodayStatistics();
    
    // 更新标签
    accuracyLabel->setText(QString::number(accuracy, 'f', 1) + "%");
    totalWordsLabel->setText(QString::number(todayStats.correctCount + todayStats.wrongCount));
    wrongWordsLabel->setText(QString::number(wrongWordsCount));
}

void StatisticsView::handleTimeRangeChange(const QString& range)
{
    updateStats();
}

void StatisticsView::exportStats()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "导出统计数据", "", "CSV 文件 (*.csv)");
        
    if (fileName.isEmpty())
        return;
        
    // TODO: 实现统计数据导出功能
}
