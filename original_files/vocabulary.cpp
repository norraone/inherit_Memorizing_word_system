#include "vocabulary.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QVariant>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>

void Vocabulary::initializeDatabase() {
    QSqlQuery query;
    
    // 单词表
    query.exec("CREATE TABLE IF NOT EXISTS words ("
              "english TEXT PRIMARY KEY,"
              "part_of_speech TEXT,"
              "chinese TEXT,"
              "frequency INTEGER DEFAULT 0,"
              "correct_count INTEGER DEFAULT 0,"
              "total_attempts INTEGER DEFAULT 0,"
              "added_date TEXT DEFAULT CURRENT_TIMESTAMP"
              ")");
              
    // 单词释义表
    query.exec("CREATE TABLE IF NOT EXISTS word_definitions ("
              "english TEXT,"
              "definition_type TEXT,"  // definition, example, synonym, antonym
              "content TEXT,"
              "added_date TEXT DEFAULT CURRENT_TIMESTAMP,"
              "PRIMARY KEY (english, definition_type, content),"
              "FOREIGN KEY(english) REFERENCES words(english)"
              ")");
              
    // 单词分类表
    query.exec("CREATE TABLE IF NOT EXISTS word_categories ("
              "english TEXT,"
              "category TEXT,"
              "added_date TEXT DEFAULT CURRENT_TIMESTAMP,"
              "PRIMARY KEY (english, category),"
              "FOREIGN KEY(english) REFERENCES words(english)"
              ")");
}

Vocabulary::Vocabulary() {
    QSqlDatabase db = Database::getConnection();
    initializeDatabase();
}

bool Vocabulary::addWord(const Word& word) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO words "
                 "(english, part_of_speech, chinese, frequency, correct_count, total_attempts) "
                 "VALUES (?, ?, ?, ?, ?, ?)");
                 
    query.addBindValue(QString::fromStdString(word.getEnglish()));
    query.addBindValue(QString::fromStdString(word.getPartOfSpeech()));
    query.addBindValue(QString::fromStdString(word.getChinese()));
    query.addBindValue(word.getFrequency());
    query.addBindValue(word.getCorrectCount());
    query.addBindValue(word.getTotalAttempts());
    
    return query.exec();
}

bool Vocabulary::removeWord(const std::string& english) {
    QSqlQuery query;
    
    // 首先删除相关的释义和分类
    query.exec("DELETE FROM word_definitions WHERE english = '" + 
              QString::fromStdString(english) + "'");
    query.exec("DELETE FROM word_categories WHERE english = '" + 
              QString::fromStdString(english) + "'");
    
    // 然后删除单词本身
    query.prepare("DELETE FROM words WHERE english = ?");
    query.addBindValue(QString::fromStdString(english));
    
    return query.exec();
}

bool Vocabulary::modifyWord(const std::string& english, const Word& newWord) {
    // 如果是修改英文单词本身，需要特殊处理
    if (english != newWord.getEnglish()) {
        // 先删除旧单词
        removeWord(english);
        // 添加新单词
        return addWord(newWord);
    }
    
    // 否则直接更新其他信息
    return addWord(newWord);  // 使用 INSERT OR REPLACE
}

Word* Vocabulary::findWord(const std::string& english) {
    QSqlQuery query;
    query.prepare("SELECT * FROM words WHERE english = ?");
    query.addBindValue(QString::fromStdString(english));
    
    if (query.exec() && query.next()) {
        Word* word = new Word(
            query.value("english").toString().toStdString(),
            query.value("part_of_speech").toString().toStdString(),
            query.value("chinese").toString().toStdString()
        );
        
        word->setFrequency(query.value("frequency").toInt());
        word->setCorrectCount(query.value("correct_count").toInt());
        word->setTotalAttempts(query.value("total_attempts").toInt());
        
        return word;
    }
    
    return nullptr;
}

bool Vocabulary::addDefinition(const std::string& english, const std::string& newDefinition) {
    return addWordDefinition(english, "definition", newDefinition);
}

bool Vocabulary::addExample(const std::string& english, const std::string& example) {
    return addWordDefinition(english, "example", example);
}

bool Vocabulary::addSynonym(const std::string& english, const std::string& synonym) {
    return addWordDefinition(english, "synonym", synonym);
}

bool Vocabulary::addAntonym(const std::string& english, const std::string& antonym) {
    return addWordDefinition(english, "antonym", antonym);
}

bool Vocabulary::addWordDefinition(const std::string& english, 
                                 const std::string& type, 
                                 const std::string& content) {
    // 检查单词是否存在
    if (!findWord(english)) {
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO word_definitions "
                 "(english, definition_type, content) VALUES (?, ?, ?)");
                 
    query.addBindValue(QString::fromStdString(english));
    query.addBindValue(QString::fromStdString(type));
    query.addBindValue(QString::fromStdString(content));
    
    return query.exec();
}

std::vector<Word> Vocabulary::searchWords(const std::string& keyword) {
    std::vector<Word> results;
    QSqlQuery query;
    
    // 搜索英文、中文和词性
    query.prepare("SELECT * FROM words WHERE "
                 "english LIKE ? OR "
                 "chinese LIKE ? OR "
                 "part_of_speech LIKE ?");
                 
    QString pattern = "%" + QString::fromStdString(keyword) + "%";
    query.addBindValue(pattern);
    query.addBindValue(pattern);
    query.addBindValue(pattern);
    
    if (query.exec()) {
        while (query.next()) {
            Word word(
                query.value("english").toString().toStdString(),
                query.value("part_of_speech").toString().toStdString(),
                query.value("chinese").toString().toStdString()
            );
            
            word.setFrequency(query.value("frequency").toInt());
            word.setCorrectCount(query.value("correct_count").toInt());
            word.setTotalAttempts(query.value("total_attempts").toInt());
            
            results.push_back(word);
        }
    }
    
    return results;
}

std::vector<Word> Vocabulary::getRandomWords(int count) {
    std::vector<Word> allWords;
    QSqlQuery query("SELECT * FROM words");
    
    if (query.exec()) {
        while (query.next()) {
            Word word(
                query.value("english").toString().toStdString(),
                query.value("part_of_speech").toString().toStdString(),
                query.value("chinese").toString().toStdString()
            );
            
            word.setFrequency(query.value("frequency").toInt());
            word.setCorrectCount(query.value("correct_count").toInt());
            word.setTotalAttempts(query.value("total_attempts").toInt());
            
            allWords.push_back(word);
        }
    }
    
    // 随机打乱
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(allWords.begin(), allWords.end(), gen);
    
    // 返回指定数量的单词
    if (count > static_cast<int>(allWords.size())) {
        count = static_cast<int>(allWords.size());
    }
    
    return std::vector<Word>(allWords.begin(), allWords.begin() + count);
}

bool Vocabulary::importFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string english, pos, chinese;
        
        // 假设文件格式为：英文,词性,中文
        if (std::getline(iss, english, ',') &&
            std::getline(iss, pos, ',') &&
            std::getline(iss, chinese)) {
            
            Word word(english, pos, chinese);
            addWord(word);
        }
    }
    
    file.close();
    return true;
}

bool Vocabulary::exportToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    QSqlQuery query("SELECT * FROM words");
    if (query.exec()) {
        while (query.next()) {
            file << query.value("english").toString().toStdString() << ","
                 << query.value("part_of_speech").toString().toStdString() << ","
                 << query.value("chinese").toString().toStdString() << "\n";
        }
    }
    
    file.close();
    return true;
}

// 词库统计方法
int Vocabulary::getTotalWords() const {
    QSqlQuery query("SELECT COUNT(*) FROM words");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

std::vector<std::string> Vocabulary::getAllPartsOfSpeech() const {
    std::vector<std::string> parts;
    QSqlQuery query("SELECT DISTINCT part_of_speech FROM words");
    
    if (query.exec()) {
        while (query.next()) {
            parts.push_back(query.value(0).toString().toStdString());
        }
    }
    
    return parts;
}
