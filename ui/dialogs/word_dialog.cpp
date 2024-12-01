#include "word_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>

WordDialog::WordDialog(QWidget* parent)
    : QDialog(parent), isEditMode(false), currentWord() {
    setupUi();
    connectSignals();
}

WordDialog::WordDialog(QWidget* parent, const Word& word)
    : QDialog(parent), isEditMode(true), currentWord(word) {
    setupUi();
    connectSignals();
    loadWord(word);
}

void WordDialog::setupUi() {
    setWindowTitle(isEditMode ? "编辑单词" : "添加单词");
    
    auto* mainLayout = new QVBoxLayout(this);
    auto* formLayout = new QFormLayout();
    
    // Basic word information
    englishEdit = new QLineEdit(this);
    englishEdit->setEnabled(!isEditMode);  // Can't edit English word in edit mode
    
    partOfSpeechCombo = new QComboBox(this);
    partOfSpeechCombo->addItems({"n.", "v.", "adj.", "adv.", "prep.", "conj.", "pron.", "art."});
    
    chineseEdit = new QLineEdit(this);
    
    formLayout->addRow("英文:", englishEdit);
    formLayout->addRow("词性:", partOfSpeechCombo);
    formLayout->addRow("中文:", chineseEdit);
    
    // Definitions section
    auto* definitionLayout = new QHBoxLayout();
    definitionEdit = new QLineEdit(this);
    definitionTypeCombo = new QComboBox(this);
    definitionTypeCombo->addItems({"释义", "例句", "同义词", "反义词"});
    addDefinitionButton = new QPushButton("添加", this);
    
    definitionLayout->addWidget(definitionTypeCombo);
    definitionLayout->addWidget(definitionEdit);
    definitionLayout->addWidget(addDefinitionButton);
    
    definitionsList = new QListWidget(this);
    definitionsList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    formLayout->addRow("释义:", definitionLayout);
    formLayout->addRow("", definitionsList);
    
    // Categories section
    auto* categoryLayout = new QHBoxLayout();
    categoryEdit = new QLineEdit(this);
    addCategoryButton = new QPushButton("添加", this);
    
    categoryLayout->addWidget(categoryEdit);
    categoryLayout->addWidget(addCategoryButton);
    
    categoriesList = new QListWidget(this);
    categoriesList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    formLayout->addRow("分类:", categoryLayout);
    formLayout->addRow("", categoriesList);
    
    // Dialog buttons
    auto* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this);
    
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &WordDialog::onAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void WordDialog::connectSignals() {
    connect(addDefinitionButton, &QPushButton::clicked,
            this, &WordDialog::onAddDefinition);
    connect(definitionsList, &QListWidget::itemDoubleClicked,
            this, &WordDialog::onRemoveDefinition);
    connect(addCategoryButton, &QPushButton::clicked,
            this, &WordDialog::onAddCategory);
    connect(categoriesList, &QListWidget::itemDoubleClicked,
            this, &WordDialog::onRemoveCategory);
}

void WordDialog::loadWord(const Word& word) {
    englishEdit->setText(QString::fromStdString(word.getEnglish()));
    partOfSpeechCombo->setCurrentText(QString::fromStdString(word.getPartOfSpeech()));
    chineseEdit->setText(QString::fromStdString(word.getChinese()));
    
    // Load definitions
    for (const auto& def : word.getDefinitions()) {
        QString item = QString("%1: %2")
            .arg(QString::fromStdString(def.type))
            .arg(QString::fromStdString(def.content));
        definitionsList->addItem(item);
    }
    
    // Load categories
    for (const auto& category : word.getCategories()) {
        categoriesList->addItem(QString::fromStdString(category));
    }
}

void WordDialog::onAddDefinition() {
    QString type = definitionTypeCombo->currentText();
    QString content = definitionEdit->text().trimmed();
    
    if (content.isEmpty()) {
        QMessageBox::warning(this, "错误", "释义内容不能为空");
        return;
    }
    
    QString item = QString("%1: %2").arg(type).arg(content);
    definitionsList->addItem(item);
    definitionEdit->clear();
}

void WordDialog::onRemoveDefinition() {
    auto* item = definitionsList->currentItem();
    if (item) {
        delete item;
    }
}

void WordDialog::onAddCategory() {
    QString category = categoryEdit->text().trimmed();
    
    if (category.isEmpty()) {
        QMessageBox::warning(this, "错误", "分类名称不能为空");
        return;
    }
    
    // Check for duplicates
    for (int i = 0; i < categoriesList->count(); ++i) {
        if (categoriesList->item(i)->text() == category) {
            QMessageBox::warning(this, "错误", "该分类已存在");
            return;
        }
    }
    
    categoriesList->addItem(category);
    categoryEdit->clear();
}

void WordDialog::onRemoveCategory() {
    auto* item = categoriesList->currentItem();
    if (item) {
        delete item;
    }
}

void WordDialog::onAccept() {
    // Validate input
    QString english = englishEdit->text().trimmed();
    QString chinese = chineseEdit->text().trimmed();
    
    if (english.isEmpty() || chinese.isEmpty()) {
        QMessageBox::warning(this, "错误", "英文和中文释义不能为空");
        return;
    }
    
    // Create word object
    currentWord = Word(
        english.toStdString(),
        partOfSpeechCombo->currentText().toStdString(),
        chinese.toStdString()
    );
    
    // Add definitions
    for (int i = 0; i < definitionsList->count(); ++i) {
        QString item = definitionsList->item(i)->text();
        int colonPos = item.indexOf(':');
        if (colonPos != -1) {
            QString type = item.left(colonPos).trimmed();
            QString content = item.mid(colonPos + 1).trimmed();
            currentWord.addDefinition(type.toStdString(), content.toStdString());
        }
    }
    
    // Add categories
    for (int i = 0; i < categoriesList->count(); ++i) {
        QString category = categoriesList->item(i)->text();
        currentWord.addCategory(category.toStdString());
    }
    
    accept();
}

Word WordDialog::getWord() const {
    return currentWord;
}
