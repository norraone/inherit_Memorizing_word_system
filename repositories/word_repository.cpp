#include "word_repository.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

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
        qDebug() << "Error saving word: " << e.what();
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
        qDebug() << "Error updating word: " << e.what();
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
        qDebug() << "Error removing word: " << e.what();
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

std::vector<WordRepository::WordStats> WordRepository::getWordStats(const std::string& username) {
    QSqlQuery query(db);
    query.prepare("SELECT w.english, COUNT(a.id) as attempts, "
                 "SUM(CASE WHEN a.correct THEN 1 ELSE 0 END) as correct_count, "
                 "w.frequency "
                 "FROM words w "
                 "LEFT JOIN attempts a ON w.id = a.word_id AND a.username = :username "
                 "GROUP BY w.id");
    query.bindValue(":username", QString::fromStdString(username));
    
    std::vector<WordStats> stats;
    if (query.exec()) {
        while (query.next()) {
            WordStats stat;
            stat.english = query.value("english").toString().toStdString();
            stat.attempts = query.value("attempts").toInt();
            stat.correctCount = query.value("correct_count").toInt();
            stat.frequency = query.value("frequency").toInt();
            stat.accuracy = stat.attempts > 0 ? static_cast<double>(stat.correctCount) / stat.attempts : 0.0;
            stats.push_back(stat);
        }
    }
    return stats;
}

std::vector<WordRepository::DailyStats> WordRepository::getDailyStats(
    const std::string& username, 
    const std::chrono::system_clock::time_point& date) {
    
    QSqlQuery query(db);
    query.prepare("SELECT DATE(a.attempt_date) as date, "
                 "COUNT(DISTINCT w.id) as words_learned, "
                 "COUNT(a.id) as words_reviewed, "
                 "AVG(CASE WHEN a.correct THEN 1 ELSE 0 END) as accuracy "
                 "FROM attempts a "
                 "JOIN words w ON w.id = a.word_id "
                 "WHERE a.username = :username "
                 "AND a.attempt_date >= :date "
                 "GROUP BY DATE(a.attempt_date)");
                 
    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":date", QDateTime::fromSecsSinceEpoch(
        std::chrono::system_clock::to_time_t(date)));
    
    std::vector<DailyStats> stats;
    if (query.exec()) {
        while (query.next()) {
            DailyStats stat;
            stat.date = std::chrono::system_clock::from_time_t(
                query.value("date").toDateTime().toSecsSinceEpoch());
            stat.wordsLearned = query.value("words_learned").toInt();
            stat.wordsReviewed = query.value("words_reviewed").toInt();
            stat.accuracy = query.value("accuracy").toDouble();
            stats.push_back(stat);
        }
    }
    return stats;
}

std::map<std::string, int> WordRepository::getWordCountByCategory() {
    QSqlQuery query(db);
    query.prepare("SELECT category, COUNT(*) as count "
                 "FROM word_categories "
                 "GROUP BY category");
    
    std::map<std::string, int> counts;
    if (query.exec()) {
        while (query.next()) {
            std::string category = query.value("category").toString().toStdString();
            int count = query.value("count").toInt();
            counts[category] = count;
        }
    }
    return counts;
}

std::vector<WordRepository::WordStats> WordRepository::getMostReviewedWords(int limit) {
    QSqlQuery query(db);
    query.prepare("SELECT w.english, COUNT(a.id) as attempts, "
                 "SUM(CASE WHEN a.correct THEN 1 ELSE 0 END) as correct_count, "
                 "w.frequency "
                 "FROM words w "
                 "LEFT JOIN attempts a ON w.id = a.word_id "
                 "GROUP BY w.id "
                 "ORDER BY attempts DESC "
                 "LIMIT :limit");
    query.bindValue(":limit", limit);
    
    std::vector<WordStats> stats;
    if (query.exec()) {
        while (query.next()) {
            WordStats stat;
            stat.english = query.value("english").toString().toStdString();
            stat.attempts = query.value("attempts").toInt();
            stat.correctCount = query.value("correct_count").toInt();
            stat.frequency = query.value("frequency").toInt();
            stat.accuracy = stat.attempts > 0 ? static_cast<double>(stat.correctCount) / stat.attempts : 0.0;
            stats.push_back(stat);
        }
    }
    return stats;
}

int WordRepository::getTotalReviewTime(const std::string& username) {
    QSqlQuery query(db);
    query.prepare("SELECT SUM(review_time_seconds) as total_time "
                 "FROM attempts "
                 "WHERE username = :username");
    query.bindValue(":username", QString::fromStdString(username));
    
    if (query.exec() && query.next()) {
        return query.value("total_time").toInt();
    }
    return 0;
}

std::vector<Word> WordRepository::getAllWords() {
    std::vector<Word> words;
    QSqlQuery query(db);
    query.prepare("SELECT english FROM words");
    
    if (query.exec()) {
        while (query.next()) {
            auto word = findByEnglish(query.value(0).toString().toStdString());
            if (word) {
                words.push_back(*word);
            }
        }
    }
    
    return words;
}
