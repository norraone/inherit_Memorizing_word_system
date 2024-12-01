#ifndef VOCABULARY_VIEW_H
#define VOCABULARY_VIEW_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include "../../services/word_service.h"

class VocabularyView : public QWidget {
    Q_OBJECT

private:
    WordService* wordService;
    QTableView* wordTable;
    QLineEdit* searchBox;
    QComboBox* categoryFilter;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deleteButton;

    void setupUi();
    void connectSignals();
    void refreshWordList();
    void showAddWordDialog();
    void showEditWordDialog();
    void confirmDeleteWord();

private slots:
    void onSearchTextChanged(const QString& text);
    void onCategoryFilterChanged(const QString& category);
    void onAddButtonClicked();
    void onEditButtonClicked();
    void onDeleteButtonClicked();
    void onSelectionChanged();

signals:
    void wordAdded(const Word& word);
    void wordUpdated(const Word& word);
    void wordDeleted(const std::string& english);

public:
    explicit VocabularyView(WordService* service, QWidget* parent = nullptr);
    ~VocabularyView() override = default;
};

#endif // VOCABULARY_VIEW_H
