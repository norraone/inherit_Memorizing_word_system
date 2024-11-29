#include "VocabularyView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMessageBox>

VocabularyView::VocabularyView(Vocabulary* vocab, QWidget* parent) 
    : QWidget(parent), vocabulary(vocab) {
    setupUI();
    connectSignals();
    refreshWordList();
}

void VocabularyView::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    
    // 工具栏
    auto* toolbarLayout = new QHBoxLayout();
    
    // 搜索框
    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("搜索单词...");
    searchInput->setMinimumWidth(200);
    
    // 分类过滤
    categoryFilter = new QComboBox(this);
    categoryFilter->addItem("所有分类");
    categoryFilter->addItems(QStringList::fromStdVector(vocabulary->getAllCategories()));
    
    // 按钮
    addWordButton = new QPushButton("添加单词", this);
    importButton = new QPushButton("导入", this);
    exportButton = new QPushButton("导出", this);
    
    toolbarLayout->addWidget(searchInput);
    toolbarLayout->addWidget(categoryFilter);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(addWordButton);
    toolbarLayout->addWidget(importButton);
    toolbarLayout->addWidget(exportButton);
    
    mainLayout->addLayout(toolbarLayout);
    
    // 单词表格
    wordTable = new QTableView(this);
    wordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    wordTable->setSelectionMode(QAbstractItemView::SingleSelection);
    wordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    wordTable->horizontalHeader()->setStretchLastSection(true);
    wordTable->verticalHeader()->hide();
    
    auto* model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"英文", "词性", "中文", "熟练度", "分类"});
    wordTable->setModel(model);
    
    mainLayout->addWidget(wordTable);
    
    // 设置样式
    setStyleSheet(R"(
        QWidget {
            background-color: white;
        }
        QLineEdit, QComboBox {
            padding: 6px;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        QPushButton {
            padding: 6px 12px;
            border: none;
            border-radius: 4px;
            background-color: #2196F3;
            color: white;
        }
        QPushButton:hover {
            background-color: #1976D2;
        }
        QTableView {
            border: 1px solid #ddd;
            border-radius: 4px;
            selection-background-color: #E3F2FD;
            selection-color: #000;
        }
        QHeaderView::section {
            background-color: #f5f5f5;
            padding: 6px;
            border: none;
            border-right: 1px solid #ddd;
        }
    )");
}

void VocabularyView::connectSignals() {
    connect(searchInput, &QLineEdit::textChanged, this, &VocabularyView::handleSearch);
    connect(categoryFilter, &QComboBox::currentTextChanged, this, &VocabularyView::handleCategoryChange);
    connect(addWordButton, &QPushButton::clicked, this, &VocabularyView::handleAddWord);
    connect(importButton, &QPushButton::clicked, this, &VocabularyView::handleImport);
    connect(exportButton, &QPushButton::clicked, this, &VocabularyView::handleExport);
}

void VocabularyView::refreshWordList() {
    auto* model = qobject_cast<QStandardItemModel*>(wordTable->model());
    model->removeRows(0, model->rowCount());
    
    std::vector<Word> words;
    QString category = categoryFilter->currentText();
    
    if (category == "所有分类") {
        words = vocabulary->searchWords(searchInput->text().toStdString());
    } else {
        words = vocabulary->getWordsByCategory(category.toStdString());
    }
    
    for (const auto& word : words) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::fromStdString(word.getEnglish()))
            << new QStandardItem(QString::fromStdString(word.getPartOfSpeech()))
            << new QStandardItem(QString::fromStdString(word.getChinese()))
            << new QStandardItem(QString::number(word.getMastery()))
            << new QStandardItem(QString::fromStdString(word.getCategory()));
        model->appendRow(row);
    }
}

void VocabularyView::handleSearch() {
    refreshWordList();
}

void VocabularyView::handleCategoryChange(const QString& category) {
    refreshWordList();
}

void VocabularyView::handleAddWord() {
    // TODO: 实现添加单词对话框
}

void VocabularyView::handleImport() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "导入词库",
        QString(),
        "文本文件 (*.txt);;CSV文件 (*.csv);;所有文件 (*.*)"
    );
    
    if (!filename.isEmpty()) {
        if (vocabulary->importFromFile(filename.toStdString())) {
            QMessageBox::information(this, "成功", "词库导入成功！");
            refreshWordList();
        } else {
            QMessageBox::warning(this, "错误", "词库导入失败，请检查文件格式。");
        }
    }
}

void VocabularyView::handleExport() {
    QString filename = QFileDialog::getSaveFileName(
        this,
        "导出词库",
        QString(),
        "CSV文件 (*.csv);;文本文件 (*.txt)"
    );
    
    if (!filename.isEmpty()) {
        if (vocabulary->exportToFile(filename.toStdString())) {
            QMessageBox::information(this, "成功", "词库导出成功！");
        } else {
            QMessageBox::warning(this, "错误", "词库导出失败。");
        }
    }
}
