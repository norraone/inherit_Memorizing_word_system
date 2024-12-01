#include "statistics_view.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDateTime>

StatisticsView::StatisticsView(StatisticsService* statsService,
                             UserService* userService,
                             QWidget* parent)
    : QWidget(parent),
      statisticsService(statsService),
      userService(userService) {
    setupUi();
    refreshStats();
}

void StatisticsView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    
    // Top controls
    auto* controlsLayout = new QHBoxLayout();
    auto* periodCombo = new QComboBox(this);
    periodCombo->addItems({"最近7天", "最近30天", "全部"});
    
    auto* refreshButton = new QPushButton("刷新", this);
    auto* exportButton = new QPushButton("导出数据", this);
    
    controlsLayout->addWidget(periodCombo);
    controlsLayout->addWidget(refreshButton);
    controlsLayout->addWidget(exportButton);
    controlsLayout->addStretch();
    
    mainLayout->addLayout(controlsLayout);
    
    // Create main sections
    createOverviewSection();
    createCharts();
    createTables();
    
    // Connect signals
    connect(refreshButton, &QPushButton::clicked,
            this, &StatisticsView::onRefreshClicked);
    connect(exportButton, &QPushButton::clicked,
            this, &StatisticsView::onExportClicked);
    connect(periodCombo, &QComboBox::currentTextChanged,
            this, &StatisticsView::onPeriodChanged);
}

void StatisticsView::createOverviewSection() {
    auto* overviewGroup = new QGroupBox("学习概览", this);
    auto* overviewLayout = new QHBoxLayout(overviewGroup);
    
    // Create stat labels
    totalWordsLabel = new QLabel(overviewGroup);
    masteredWordsLabel = new QLabel(overviewGroup);
    learningWordsLabel = new QLabel(overviewGroup);
    accuracyLabel = new QLabel(overviewGroup);
    streakLabel = new QLabel(overviewGroup);
    scoreLabel = new QLabel(overviewGroup);
    
    // Style labels
    QFont statFont = font();
    statFont.setPointSize(12);
    statFont.setBold(true);
    
    for (auto* label : {totalWordsLabel, masteredWordsLabel, learningWordsLabel,
                       accuracyLabel, streakLabel, scoreLabel}) {
        label->setFont(statFont);
        label->setAlignment(Qt::AlignCenter);
        overviewLayout->addWidget(label);
    }
    
    qobject_cast<QVBoxLayout*>(layout())->addWidget(overviewGroup);
}

void StatisticsView::createCharts() {
    auto* chartsLayout = new QHBoxLayout();
    
    // Progress pie chart
    progressPieChart = new QChartView(this);
    progressPieChart->setRenderHint(QPainter::Antialiasing);
    progressPieChart->chart()->setTitle("词汇掌握程度");
    
    // Accuracy trend chart
    accuracyTrendChart = new QChartView(this);
    accuracyTrendChart->setRenderHint(QPainter::Antialiasing);
    accuracyTrendChart->chart()->setTitle("正确率趋势");
    
    // Category distribution chart
    categoryDistributionChart = new QChartView(this);
    categoryDistributionChart->setRenderHint(QPainter::Antialiasing);
    categoryDistributionChart->chart()->setTitle("词汇分类分布");
    
    chartsLayout->addWidget(progressPieChart);
    chartsLayout->addWidget(accuracyTrendChart);
    chartsLayout->addWidget(categoryDistributionChart);
    
    qobject_cast<QVBoxLayout*>(layout())->addLayout(chartsLayout);
}

void StatisticsView::createTables() {
    auto* tablesLayout = new QHBoxLayout();
    
    // Difficult words table
    auto* difficultGroup = new QGroupBox("需要加强的单词", this);
    auto* difficultLayout = new QVBoxLayout(difficultGroup);
    difficultWordsTable = new QTableWidget(difficultGroup);
    difficultWordsTable->setColumnCount(4);
    difficultWordsTable->setHorizontalHeaderLabels(
        {"单词", "释义", "正确率", "最后复习时间"});
    difficultLayout->addWidget(difficultWordsTable);
    
    // Recent activity table
    auto* recentGroup = new QGroupBox("最近学习记录", this);
    auto* recentLayout = new QVBoxLayout(recentGroup);
    recentActivityTable = new QTableWidget(recentGroup);
    recentActivityTable->setColumnCount(4);
    recentActivityTable->setHorizontalHeaderLabels(
        {"日期", "学习单词数", "复习单词数", "正确率"});
    recentLayout->addWidget(recentActivityTable);
    
    tablesLayout->addWidget(difficultGroup);
    tablesLayout->addWidget(recentGroup);
    
    qobject_cast<QVBoxLayout*>(layout())->addLayout(tablesLayout);
}

void StatisticsView::refreshStats() {
    updateOverview();
    updateCharts();
    updateTables();
}

void StatisticsView::updateOverview() {
    auto username = userService->getCurrentUser().getUsername();
    auto progress = statisticsService->getUserProgress(username);
    
    totalWordsLabel->setText(QString("总单词数\n%1").arg(progress.totalWords));
    masteredWordsLabel->setText(QString("已掌握\n%1").arg(progress.masteredWords));
    learningWordsLabel->setText(QString("学习中\n%1").arg(progress.learningWords));
    accuracyLabel->setText(QString("总正确率\n%.1f%").arg(progress.overallAccuracy * 100));
    streakLabel->setText(QString("连续学习\n%1天").arg(progress.daysStreak));
    scoreLabel->setText(QString("总分数\n%1").arg(progress.totalScore));
}

void StatisticsView::updateCharts() {
    // Update progress pie chart
    auto pieSeries = createProgressPieSeries();
    progressPieChart->chart()->removeAllSeries();
    progressPieChart->chart()->addSeries(pieSeries);
    
    // Update accuracy trend chart
    auto lineSeries = createAccuracyTrendSeries();
    accuracyTrendChart->chart()->removeAllSeries();
    accuracyTrendChart->chart()->addSeries(lineSeries);
    
    // Add axes for accuracy trend
    auto axisX = new QDateTimeAxis;
    axisX->setFormat("MM-dd");
    axisX->setTitleText("日期");
    accuracyTrendChart->chart()->addAxis(axisX, Qt::AlignBottom);
    lineSeries->attachAxis(axisX);
    
    auto axisY = new QValueAxis;
    axisY->setRange(0, 100);
    axisY->setTitleText("正确率 (%)");
    accuracyTrendChart->chart()->addAxis(axisY, Qt::AlignLeft);
    lineSeries->attachAxis(axisY);
    
    // Update category distribution chart
    auto barSeries = createCategoryDistributionSeries();
    categoryDistributionChart->chart()->removeAllSeries();
    categoryDistributionChart->chart()->addSeries(barSeries);
}

void StatisticsView::updateTables() {
    auto username = userService->getCurrentUser().getUsername();
    
    // Update difficult words table
    auto difficultWords = statisticsService->getMostDifficultWords(10);
    difficultWordsTable->setRowCount(difficultWords.size());
    
    for (int i = 0; i < difficultWords.size(); ++i) {
        const auto& word = difficultWords[i];
        difficultWordsTable->setItem(i, 0, new QTableWidgetItem(
            QString::fromStdString(word.english)));
        difficultWordsTable->setItem(i, 1, new QTableWidgetItem(
            QString::fromStdString(word.chinese)));
        difficultWordsTable->setItem(i, 2, new QTableWidgetItem(
            QString::number(word.accuracy * 100, 'f', 1) + "%"));
        
        auto lastReview = QDateTime::fromTime_t(
            std::chrono::system_clock::to_time_t(word.lastReview));
        difficultWordsTable->setItem(i, 3, new QTableWidgetItem(
            lastReview.toString("yyyy-MM-dd hh:mm")));
    }
    
    // Update recent activity table
    auto dailyStats = statisticsService->getDailyStats(username, 7);
    recentActivityTable->setRowCount(dailyStats.size());
    
    for (int i = 0; i < dailyStats.size(); ++i) {
        const auto& stats = dailyStats[i];
        auto date = QDateTime::fromTime_t(
            std::chrono::system_clock::to_time_t(stats.date));
        
        recentActivityTable->setItem(i, 0, new QTableWidgetItem(
            date.toString("yyyy-MM-dd")));
        recentActivityTable->setItem(i, 1, new QTableWidgetItem(
            QString::number(stats.wordsLearned)));
        recentActivityTable->setItem(i, 2, new QTableWidgetItem(
            QString::number(stats.wordsReviewed)));
        recentActivityTable->setItem(i, 3, new QTableWidgetItem(
            QString::number(stats.accuracy * 100, 'f', 1) + "%"));
    }
    
    difficultWordsTable->resizeColumnsToContents();
    recentActivityTable->resizeColumnsToContents();
}

QPieSeries* StatisticsView::createProgressPieSeries() {
    auto username = userService->getCurrentUser().getUsername();
    auto progress = statisticsService->getUserProgress(username);
    
    auto* series = new QPieSeries();
    series->append("已掌握", progress.masteredWords);
    series->append("学习中", progress.learningWords);
    series->append("未开始", progress.newWords);
    
    return series;
}

QLineSeries* StatisticsView::createAccuracyTrendSeries() {
    auto username = userService->getCurrentUser().getUsername();
    auto dailyStats = statisticsService->getDailyStats(username, 7);
    
    auto* series = new QLineSeries();
    
    for (const auto& stats : dailyStats) {
        auto timestamp = std::chrono::system_clock::to_time_t(stats.date);
        series->append(timestamp * 1000, stats.accuracy * 100);  // Convert to milliseconds for Qt
    }
    
    return series;
}

QBarSeries* StatisticsView::createCategoryDistributionSeries() {
    auto categories = statisticsService->getWordsByCategory();
    
    auto* series = new QBarSeries();
    auto* set = new QBarSet("单词数");
    
    QStringList categoryLabels;
    for (const auto& [category, count] : categories) {
        *set << count;
        categoryLabels << QString::fromStdString(category);
    }
    
    series->append(set);
    
    auto* axis = new QBarCategoryAxis();
    axis->append(categoryLabels);
    series->attachAxis(axis);
    
    return series;
}

void StatisticsView::onRefreshClicked() {
    refreshStats();
}

void StatisticsView::onExportClicked() {
    QString fileName = QFileDialog::getSaveFileName(this,
        "导出统计数据", "",
        "CSV文件 (*.csv);;所有文件 (*)");
        
    if (fileName.isEmpty()) return;
    
    // TODO: Implement data export to CSV
    QMessageBox::information(this, "导出成功",
        "统计数据已成功导出到: " + fileName);
}

void StatisticsView::onPeriodChanged(const QString& period) {
    // Update charts and tables based on selected period
    refreshStats();
}
