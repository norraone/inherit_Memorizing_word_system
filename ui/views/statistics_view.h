#ifndef STATISTICS_VIEW_H
#define STATISTICS_VIEW_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QChartView>
#include <QPieSeries>
#include <QLineSeries>
#include <QBarSeries>
#include "../../services/statistics_service.h"
#include "../../services/user_service.h"

QT_CHARTS_USE_NAMESPACE

class StatisticsView : public QWidget {
    Q_OBJECT

private:
    StatisticsService* statisticsService;
    UserService* userService;
    
    // Overview widgets
    QLabel* totalWordsLabel;
    QLabel* masteredWordsLabel;
    QLabel* learningWordsLabel;
    QLabel* accuracyLabel;
    QLabel* streakLabel;
    QLabel* scoreLabel;
    
    // Chart views
    QChartView* progressPieChart;
    QChartView* accuracyTrendChart;
    QChartView* categoryDistributionChart;
    
    // Tables
    QTableWidget* difficultWordsTable;
    QTableWidget* recentActivityTable;
    
    void setupUi();
    void createOverviewSection();
    void createCharts();
    void createTables();
    
    // Chart creation helpers
    QPieSeries* createProgressPieSeries();
    QLineSeries* createAccuracyTrendSeries();
    QBarSeries* createCategoryDistributionSeries();
    
    // Data update methods
    void updateOverview();
    void updateCharts();
    void updateTables();
    
private slots:
    void onRefreshClicked();
    void onExportClicked();
    void onPeriodChanged(const QString& period);

public:
    explicit StatisticsView(StatisticsService* statsService,
                          UserService* userService,
                          QWidget* parent = nullptr);
    ~StatisticsView() override = default;
    
    void refreshStats();
};

#endif // STATISTICS_VIEW_H
