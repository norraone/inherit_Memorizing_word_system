#ifndef WORD_DIALOG_H
#define WORD_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include "../../models/word.h"

class WordDialog : public QDialog {
    Q_OBJECT

private:
    QLineEdit* englishEdit;
    QComboBox* partOfSpeechCombo;
    QLineEdit* chineseEdit;
    QLineEdit* definitionEdit;
    QComboBox* definitionTypeCombo;
    QPushButton* addDefinitionButton;
    QListWidget* definitionsList;
    QLineEdit* categoryEdit;
    QPushButton* addCategoryButton;
    QListWidget* categoriesList;
    
    Word currentWord;
    bool isEditMode;
    
    void setupUi();
    void connectSignals();
    void loadWord(const Word& word);
    void updateDefinitionsList();
    void updateCategoriesList();

private slots:
    void onAddDefinition();
    void onRemoveDefinition();
    void onAddCategory();
    void onRemoveCategory();
    void onAccept();

public:
    explicit WordDialog(QWidget* parent = nullptr);
    explicit WordDialog(QWidget* parent, const Word& word);
    ~WordDialog() override = default;
    
    Word getWord() const;
};

#endif // WORD_DIALOG_H
