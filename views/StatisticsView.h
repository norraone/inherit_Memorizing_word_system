#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include "../statistics.h"

class StatisticsView : public QWidget {
    Q_OBJECT

private:
    Statistics* statistics;
    QLabel* accuracyLabel;
    QLabel* totalWordsLabel;
    QLabel* wrongWordsLabel;
    QComboBox* timeRangeSelector;
    
    void setupUI();
    void connectSignals();
    void updateStats();

private slots:
    void handleTimeRangeChange(const QString& range);
    void exportStats();

public:
    explicit StatisticsView(Statistics* stats, QWidget* parent = nullptr);
};

#endif // STATISTICSVIEW_H
