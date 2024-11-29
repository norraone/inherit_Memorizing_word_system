#ifndef VOCABULARYVIEW_H
#define VOCABULARYVIEW_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include "../vocabulary.h"

class VocabularyView : public QWidget {
    Q_OBJECT

private:
    Vocabulary* vocabulary;
    QTableView* wordTable;
    QLineEdit* searchInput;
    QComboBox* categoryFilter;
    QPushButton* addWordButton;
    QPushButton* importButton;
    QPushButton* exportButton;

    void setupUI();
    void connectSignals();
    void refreshWordList();

private slots:
    void handleSearch();
    void handleAddWord();
    void handleImport();
    void handleExport();
    void handleCategoryChange(const QString& category);

public:
    explicit VocabularyView(Vocabulary* vocab, QWidget* parent = nullptr);
};

#endif // VOCABULARYVIEW_H
