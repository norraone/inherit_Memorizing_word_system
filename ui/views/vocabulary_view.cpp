#include "vocabulary_view.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItemModel>
#include "../dialogs/word_dialog.h"

VocabularyView::VocabularyView(WordService* service, QWidget* parent)
    : QWidget(parent), wordService(service) {
    setupUi();
    connectSignals();
    refreshWordList();
}

void VocabularyView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    
    // Search and filter bar
    auto* toolBar = new QHBoxLayout();
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("搜索单词...");
    categoryFilter = new QComboBox(this);
    categoryFilter->addItem("所有分类");
    // TODO: Load categories from database
    
    toolBar->addWidget(searchBox);
    toolBar->addWidget(categoryFilter);
    
    // Word table
    wordTable = new QTableView(this);
    wordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    wordTable->setSelectionMode(QAbstractItemView::SingleSelection);
    wordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    wordTable->horizontalHeader()->setStretchLastSection(true);
    
    // Action buttons
    auto* buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("添加单词", this);
    editButton = new QPushButton("编辑", this);
    deleteButton = new QPushButton("删除", this);
    
    editButton->setEnabled(false);
    deleteButton->setEnabled(false);
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(toolBar);
    mainLayout->addWidget(wordTable);
    mainLayout->addLayout(buttonLayout);
}

void VocabularyView::connectSignals() {
    connect(searchBox, &QLineEdit::textChanged,
            this, &VocabularyView::onSearchTextChanged);
    connect(categoryFilter, &QComboBox::currentTextChanged,
            this, &VocabularyView::onCategoryFilterChanged);
    connect(addButton, &QPushButton::clicked,
            this, &VocabularyView::onAddButtonClicked);
    connect(editButton, &QPushButton::clicked,
            this, &VocabularyView::onEditButtonClicked);
    connect(deleteButton, &QPushButton::clicked,
            this, &VocabularyView::onDeleteButtonClicked);
    connect(wordTable->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &VocabularyView::onSelectionChanged);
}

void VocabularyView::refreshWordList() {
    auto* model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"英文", "词性", "中文", "正确率", "复习次数"});
    
    // TODO: Get filtered words based on search and category
    std::vector<Word> words = wordService->getAllWords();
    
    for (const auto& word : words) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::fromStdString(word.getEnglish())));
        row.append(new QStandardItem(QString::fromStdString(word.getPartOfSpeech())));
        row.append(new QStandardItem(QString::fromStdString(word.getChinese())));
        row.append(new QStandardItem(QString::number(word.getStats().getAccuracy() * 100, 'f', 1) + "%"));
        row.append(new QStandardItem(QString::number(word.getStats().totalAttempts)));
        model->appendRow(row);
    }
    
    wordTable->setModel(model);
    wordTable->resizeColumnsToContents();
}

void VocabularyView::showAddWordDialog() {
    WordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            Word word = dialog.getWord();
            if (wordService->addWord(word)) {
                emit wordAdded(word);
                refreshWordList();
            } else {
                QMessageBox::warning(this, "错误", "添加单词失败");
            }
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "错误", QString("添加单词失败: %1").arg(e.what()));
        }
    }
}

void VocabularyView::showEditWordDialog() {
    auto selection = wordTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) return;
    
    QString english = wordTable->model()->data(selection[0]).toString();
    auto wordOpt = wordService->getWord(english.toStdString());
    
    if (!wordOpt) {
        QMessageBox::warning(this, "错误", "找不到选中的单词");
        return;
    }
    
    WordDialog dialog(this, *wordOpt);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            Word word = dialog.getWord();
            if (wordService->updateWord(word)) {
                emit wordUpdated(word);
                refreshWordList();
            } else {
                QMessageBox::warning(this, "错误", "更新单词失败");
            }
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "错误", QString("更新单词失败: %1").arg(e.what()));
        }
    }
}

void VocabularyView::confirmDeleteWord() {
    auto selection = wordTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) return;
    
    QString english = wordTable->model()->data(selection[0]).toString();
    
    auto reply = QMessageBox::question(this, "确认删除",
        QString("确定要删除单词 '%1' 吗？").arg(english),
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        try {
            if (wordService->deleteWord(english.toStdString())) {
                emit wordDeleted(english.toStdString());
                refreshWordList();
            } else {
                QMessageBox::warning(this, "错误", "删除单词失败");
            }
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "错误", QString("删除单词失败: %1").arg(e.what()));
        }
    }
}

void VocabularyView::onSearchTextChanged(const QString& /*text*/) {
    // TODO: Implement search filtering
    refreshWordList();
}

void VocabularyView::onCategoryFilterChanged(const QString& /*category*/) {
    // TODO: Implement category filtering
    refreshWordList();
}

void VocabularyView::onAddButtonClicked() {
    showAddWordDialog();
}

void VocabularyView::onEditButtonClicked() {
    showEditWordDialog();
}

void VocabularyView::onDeleteButtonClicked() {
    confirmDeleteWord();
}

void VocabularyView::onSelectionChanged() {
    bool hasSelection = !wordTable->selectionModel()->selectedRows().isEmpty();
    editButton->setEnabled(hasSelection);
    deleteButton->setEnabled(hasSelection);
}
