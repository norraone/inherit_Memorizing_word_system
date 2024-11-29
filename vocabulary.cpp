#include "vocabulary.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <random>
#include <algorithm>
#include <stdexcept>

Vocabulary::Vocabulary() {
    initializeDatabase();
}

void Vocabulary::initializeDatabase() {
    if (!QSqlDatabase::contains("vocabulary_connection")) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "vocabulary_connection");
        db.setDatabaseName("vocabulary.db");
        
        if (!db.open()) {
            throw std::runtime_error("Failed to open vocabulary database: " + db.lastError().text().toStdString());
        }

        QSqlQuery query(db);
        // Create main vocabulary table
        query.exec("CREATE TABLE IF NOT EXISTS vocabulary ("
                  "english TEXT PRIMARY KEY,"
                  "part_of_speech TEXT,"
                  "chinese TEXT,"
                  "frequency INTEGER DEFAULT 0,"
                  "correct_count INTEGER DEFAULT 0,"
                  "total_attempts INTEGER DEFAULT 0"
                  ")");

        // Create wrong words table (links to main vocabulary)
        query.exec("CREATE TABLE IF NOT EXISTS wrong_words ("
                  "english TEXT PRIMARY KEY,"
                  "added_date DATETIME DEFAULT CURRENT_TIMESTAMP,"
                  "FOREIGN KEY (english) REFERENCES vocabulary(english)"
                  ")");
    }
}

void Vocabulary::addWord(const Word& word) {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    query.prepare("INSERT OR REPLACE INTO vocabulary "
                 "(english, part_of_speech, chinese, frequency, correct_count, total_attempts) "
                 "VALUES (?, ?, ?, ?, ?, ?)");
    
    query.addBindValue(QString::fromStdString(word.getEnglish()));
    query.addBindValue(QString::fromStdString(word.getPartOfSpeech()));
    query.addBindValue(QString::fromStdString(word.getChinese()));
    query.addBindValue(word.getFrequency());
    query.addBindValue(word.getCorrectCount());
    query.addBindValue(word.getTotalAttempts());
    
    if (!query.exec()) {
        throw std::runtime_error("Failed to add word: " + query.lastError().text().toStdString());
    }
}

void Vocabulary::removeWord(const std::string& english) {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    // First remove from wrong_words if it exists there
    query.prepare("DELETE FROM wrong_words WHERE english = ?");
    query.addBindValue(QString::fromStdString(english));
    query.exec();
    
    // Then remove from vocabulary
    query.prepare("DELETE FROM vocabulary WHERE english = ?");
    query.addBindValue(QString::fromStdString(english));
    
    if (!query.exec()) {
        throw std::runtime_error("Failed to remove word: " + query.lastError().text().toStdString());
    }
}

void Vocabulary::modifyWord(const std::string& english, const Word& newWord) {
    // Simply use addWord as it handles REPLACE
    addWord(newWord);
}

Word* Vocabulary::findWord(const std::string& english) {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    query.prepare("SELECT * FROM vocabulary WHERE english = ?");
    query.addBindValue(QString::fromStdString(english));
    
    if (query.exec() && query.next()) {
        Word* word = new Word(
            english,
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

std::vector<Word> Vocabulary::getRandomWords(int count) {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    // Get total count of words
    query.exec("SELECT COUNT(*) FROM vocabulary");
    query.next();
    int totalWords = query.value(0).toInt();
    count = std::min(count, totalWords);
    
    // Get random words
    query.prepare("SELECT * FROM vocabulary ORDER BY RANDOM() LIMIT ?");
    query.addBindValue(count);
    
    std::vector<Word> words;
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
            words.push_back(word);
        }
    }
    return words;
}

void Vocabulary::addToWrongWords(const Word& word) {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    // First ensure word exists in vocabulary
    addWord(word);
    
    // Then add to wrong_words
    query.prepare("INSERT OR REPLACE INTO wrong_words (english) VALUES (?)");
    query.addBindValue(QString::fromStdString(word.getEnglish()));
    
    if (!query.exec()) {
        throw std::runtime_error("Failed to add to wrong words: " + query.lastError().text().toStdString());
    }
}

void Vocabulary::removeFromWrongWords(const Word& word) {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    query.prepare("DELETE FROM wrong_words WHERE english = ?");
    query.addBindValue(QString::fromStdString(word.getEnglish()));
    
    if (!query.exec()) {
        throw std::runtime_error("Failed to remove from wrong words: " + query.lastError().text().toStdString());
    }
}

std::vector<Word> Vocabulary::getWrongWords() {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    query.prepare("SELECT v.* FROM vocabulary v "
                 "INNER JOIN wrong_words w ON v.english = w.english "
                 "ORDER BY w.added_date DESC");
    
    std::vector<Word> wrongWords;
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
            wrongWords.push_back(word);
        }
    }
    return wrongWords;
}

int Vocabulary::getTotalWords() const {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    query.exec("SELECT COUNT(*) FROM vocabulary");
    query.next();
    return query.value(0).toInt();
}

int Vocabulary::getWrongWordsCount() const {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    query.exec("SELECT COUNT(*) FROM wrong_words");
    query.next();
    return query.value(0).toInt();
}

// Additional utility functions for word statistics
std::vector<Word> Vocabulary::getMostFrequentWords(int limit) {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    query.prepare("SELECT * FROM vocabulary ORDER BY frequency DESC LIMIT ?");
    query.addBindValue(limit);
    
    std::vector<Word> words;
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
            words.push_back(word);
        }
    }
    return words;
}

std::vector<Word> Vocabulary::getMostDifficultWords(int limit) {
    QSqlDatabase db = QSqlDatabase::database("vocabulary_connection");
    QSqlQuery query(db);
    
    query.prepare("SELECT *, CAST(correct_count AS FLOAT) / NULLIF(total_attempts, 0) as success_rate "
                 "FROM vocabulary "
                 "WHERE total_attempts > 0 "
                 "ORDER BY success_rate ASC "
                 "LIMIT ?");
    query.addBindValue(limit);
    
    std::vector<Word> words;
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
            words.push_back(word);
        }
    }
    return words;
}
