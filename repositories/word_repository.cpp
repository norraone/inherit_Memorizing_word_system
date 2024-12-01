#include "word_repository.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

std::optional<Word> WordRepository::findByEnglish(const std::string& english) {
    QSqlQuery query(db);
    query.prepare("SELECT * FROM words WHERE english = ?");
    query.addBindValue(QString::fromStdString(english));
    
    if (query.exec() && query.next()) {
        Word word(
            query.value("english").toString().toStdString(),
            query.value("part_of_speech").toString().toStdString(),
            query.value("chinese").toString().toStdString()
        );
        
        // Load learning stats
        word.getStats().frequency = query.value("frequency").toInt();
        word.getStats().correctCount = query.value("correct_count").toInt();
        word.getStats().totalAttempts = query.value("total_attempts").toInt();
        
        // Load definitions
        QSqlQuery defQuery(db);
        defQuery.prepare("SELECT * FROM word_definitions WHERE english = ?");
        defQuery.addBindValue(QString::fromStdString(english));
        
        if (defQuery.exec()) {
            while (defQuery.next()) {
                word.addDefinition(
                    defQuery.value("definition_type").toString().toStdString(),
                    defQuery.value("content").toString().toStdString()
                );
            }
        }
        
        // Load categories
        QSqlQuery catQuery(db);
        catQuery.prepare("SELECT category FROM word_categories WHERE english = ?");
        catQuery.addBindValue(QString::fromStdString(english));
        
        if (catQuery.exec()) {
            while (catQuery.next()) {
                word.addCategory(catQuery.value("category").toString().toStdString());
            }
        }
        
        return word;
    }
    
    return std::nullopt;
}

std::vector<Word> WordRepository::findByCategory(const std::string& category) {
    std::vector<Word> words;
    QSqlQuery query(db);
    query.prepare(
        "SELECT w.* FROM words w "
        "INNER JOIN word_categories wc ON w.english = wc.english "
        "WHERE wc.category = ?"
    );
    query.addBindValue(QString::fromStdString(category));
    
    if (query.exec()) {
        while (query.next()) {
            auto word = findByEnglish(query.value("english").toString().toStdString());
            if (word) {
                words.push_back(*word);
            }
        }
    }
    
    return words;
}

std::vector<Word> WordRepository::findDueForReview(const std::string& username, int limit) {
    std::vector<Word> words;
    QSqlQuery query(db);
    query.prepare(
        "SELECT w.* FROM words w "
        "LEFT JOIN learning_records lr ON w.english = lr.word AND lr.username = ? "
        "WHERE lr.last_review_date IS NULL "
        "OR julianday('now') - julianday(lr.last_review_date) >= "
        "CASE "
        "   WHEN lr.mastery_level = 1 THEN 1 "  // Review after 1 day
        "   WHEN lr.mastery_level = 2 THEN 3 "  // Review after 3 days
        "   WHEN lr.mastery_level = 3 THEN 7 "  // Review after 7 days
        "   ELSE 14 "                           // Review after 14 days
        "END "
        "ORDER BY RANDOM() LIMIT ?"
    );
    query.addBindValue(QString::fromStdString(username));
    query.addBindValue(limit);
    
    if (query.exec()) {
        while (query.next()) {
            auto word = findByEnglish(query.value("english").toString().toStdString());
            if (word) {
                words.push_back(*word);
            }
        }
    }
    
    return words;
}

bool WordRepository::save(const Word& word) {
    db.transaction();
    
    try {
        QSqlQuery query(db);
        query.prepare(
            "INSERT INTO words (english, part_of_speech, chinese, frequency, "
            "correct_count, total_attempts) VALUES (?, ?, ?, ?, ?, ?)"
        );
        query.addBindValue(QString::fromStdString(word.getEnglish()));
        query.addBindValue(QString::fromStdString(word.getPartOfSpeech()));
        query.addBindValue(QString::fromStdString(word.getChinese()));
        query.addBindValue(word.getStats().frequency);
        query.addBindValue(word.getStats().correctCount);
        query.addBindValue(word.getStats().totalAttempts);
        
        if (!query.exec()) {
            throw std::runtime_error("Failed to save word");
        }
        
        // Save definitions
        for (const auto& def : word.getDefinitions()) {
            QSqlQuery defQuery(db);
            defQuery.prepare(
                "INSERT INTO word_definitions (english, definition_type, content) "
                "VALUES (?, ?, ?)"
            );
            defQuery.addBindValue(QString::fromStdString(word.getEnglish()));
            defQuery.addBindValue(QString::fromStdString(def.type));
            defQuery.addBindValue(QString::fromStdString(def.content));
            
            if (!defQuery.exec()) {
                throw std::runtime_error("Failed to save word definition");
            }
        }
        
        // Save categories
        for (const auto& category : word.getCategories()) {
            QSqlQuery catQuery(db);
            catQuery.prepare(
                "INSERT INTO word_categories (english, category) VALUES (?, ?)"
            );
            catQuery.addBindValue(QString::fromStdString(word.getEnglish()));
            catQuery.addBindValue(QString::fromStdString(category));
            
            if (!catQuery.exec()) {
                throw std::runtime_error("Failed to save word category");
            }
        }
        
        db.commit();
        return true;
    } catch (const std::exception& e) {
        db.rollback();
        return false;
    }
}

bool WordRepository::update(const Word& word) {
    db.transaction();
    
    try {
        QSqlQuery query(db);
        query.prepare(
            "UPDATE words SET part_of_speech = ?, chinese = ?, frequency = ?, "
            "correct_count = ?, total_attempts = ? WHERE english = ?"
        );
        query.addBindValue(QString::fromStdString(word.getPartOfSpeech()));
        query.addBindValue(QString::fromStdString(word.getChinese()));
        query.addBindValue(word.getStats().frequency);
        query.addBindValue(word.getStats().correctCount);
        query.addBindValue(word.getStats().totalAttempts);
        query.addBindValue(QString::fromStdString(word.getEnglish()));
        
        if (!query.exec()) {
            throw std::runtime_error("Failed to update word");
        }
        
        // Update definitions (delete and re-insert)
        QSqlQuery delDefQuery(db);
        delDefQuery.prepare("DELETE FROM word_definitions WHERE english = ?");
        delDefQuery.addBindValue(QString::fromStdString(word.getEnglish()));
        
        if (!delDefQuery.exec()) {
            throw std::runtime_error("Failed to delete word definitions");
        }
        
        for (const auto& def : word.getDefinitions()) {
            QSqlQuery defQuery(db);
            defQuery.prepare(
                "INSERT INTO word_definitions (english, definition_type, content) "
                "VALUES (?, ?, ?)"
            );
            defQuery.addBindValue(QString::fromStdString(word.getEnglish()));
            defQuery.addBindValue(QString::fromStdString(def.type));
            defQuery.addBindValue(QString::fromStdString(def.content));
            
            if (!defQuery.exec()) {
                throw std::runtime_error("Failed to update word definition");
            }
        }
        
        // Update categories (delete and re-insert)
        QSqlQuery delCatQuery(db);
        delCatQuery.prepare("DELETE FROM word_categories WHERE english = ?");
        delCatQuery.addBindValue(QString::fromStdString(word.getEnglish()));
        
        if (!delCatQuery.exec()) {
            throw std::runtime_error("Failed to delete word categories");
        }
        
        for (const auto& category : word.getCategories()) {
            QSqlQuery catQuery(db);
            catQuery.prepare(
                "INSERT INTO word_categories (english, category) VALUES (?, ?)"
            );
            catQuery.addBindValue(QString::fromStdString(word.getEnglish()));
            catQuery.addBindValue(QString::fromStdString(category));
            
            if (!catQuery.exec()) {
                throw std::runtime_error("Failed to update word category");
            }
        }
        
        db.commit();
        return true;
    } catch (const std::exception& e) {
        db.rollback();
        return false;
    }
}

bool WordRepository::remove(const std::string& english) {
    db.transaction();
    
    try {
        // Delete from word_definitions
        QSqlQuery delDefQuery(db);
        delDefQuery.prepare("DELETE FROM word_definitions WHERE english = ?");
        delDefQuery.addBindValue(QString::fromStdString(english));
        
        if (!delDefQuery.exec()) {
            throw std::runtime_error("Failed to delete word definitions");
        }
        
        // Delete from word_categories
        QSqlQuery delCatQuery(db);
        delCatQuery.prepare("DELETE FROM word_categories WHERE english = ?");
        delCatQuery.addBindValue(QString::fromStdString(english));
        
        if (!delCatQuery.exec()) {
            throw std::runtime_error("Failed to delete word categories");
        }
        
        // Delete from learning_records
        QSqlQuery delLearningQuery(db);
        delLearningQuery.prepare("DELETE FROM learning_records WHERE word = ?");
        delLearningQuery.addBindValue(QString::fromStdString(english));
        
        if (!delLearningQuery.exec()) {
            throw std::runtime_error("Failed to delete learning records");
        }
        
        // Delete from words
        QSqlQuery delWordQuery(db);
        delWordQuery.prepare("DELETE FROM words WHERE english = ?");
        delWordQuery.addBindValue(QString::fromStdString(english));
        
        if (!delWordQuery.exec()) {
            throw std::runtime_error("Failed to delete word");
        }
        
        db.commit();
        return true;
    } catch (const std::exception& e) {
        db.rollback();
        return false;
    }
}

std::vector<Word> WordRepository::getMostDifficultWords(int limit) {
    std::vector<Word> words;
    QSqlQuery query(db);
    query.prepare(
        "SELECT * FROM words "
        "WHERE total_attempts > 0 "
        "ORDER BY CAST(correct_count AS FLOAT) / total_attempts ASC "
        "LIMIT ?"
    );
    query.addBindValue(limit);
    
    if (query.exec()) {
        while (query.next()) {
            auto word = findByEnglish(query.value("english").toString().toStdString());
            if (word) {
                words.push_back(*word);
            }
        }
    }
    
    return words;
}

std::vector<Word> WordRepository::getMostFrequentWords(int limit) {
    std::vector<Word> words;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM words ORDER BY frequency DESC LIMIT ?");
    query.addBindValue(limit);
    
    if (query.exec()) {
        while (query.next()) {
            auto word = findByEnglish(query.value("english").toString().toStdString());
            if (word) {
                words.push_back(*word);
            }
        }
    }
    
    return words;
}
