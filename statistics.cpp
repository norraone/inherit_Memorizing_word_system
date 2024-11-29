#include "statistics.h"
#include <algorithm>
#include <random>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

Statistics::Statistics(User& u) 
    : user(u), totalScore(0), todayScore(0) {
    initializeDatabase();
    loadStatistics();
}

Statistics::~Statistics() {
    saveStatistics();
}

void Statistics::initializeDatabase() {
    QSqlQuery query;
    
    // 错题本表
    query.exec("CREATE TABLE IF NOT EXISTS wrong_words ("
              "username TEXT,"
              "word_english TEXT,"
              "added_date TEXT,"
              "attempt_count INTEGER DEFAULT 0,"
              "PRIMARY KEY (username, word_english),"
              "FOREIGN KEY(username) REFERENCES users(username)"
              ")");
              
    // 每日统计表
    query.exec("CREATE TABLE IF NOT EXISTS daily_statistics ("
              "username TEXT,"
              "date TEXT,"
              "score INTEGER,"
              "correct_count INTEGER,"
              "wrong_count INTEGER,"
              "PRIMARY KEY (username, date),"
              "FOREIGN KEY(username) REFERENCES users(username)"
              ")");
}

void Statistics::updateScore(bool isCorrect) {
    int points = isCorrect ? CORRECT_SCORE : WRONG_SCORE;
    totalScore += points;
    todayScore += points;
    
    // 更新每日统计
    QSqlQuery query;
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    
    query.prepare("INSERT OR REPLACE INTO daily_statistics "
                 "(username, date, score, correct_count, wrong_count) "
                 "VALUES (?, ?, ?, ?, ?)");
                 
    query.addBindValue(QString::fromStdString(user.getUsername()));
    query.addBindValue(today);
    query.addBindValue(todayScore);
    query.addBindValue(isCorrect ? 1 : 0);
    query.addBindValue(isCorrect ? 0 : 1);
    
    query.exec();
}

void Statistics::addToWrongWords(const Word& word) {
    // 检查是否已经在错题本中
    if (!isInWrongWords(word.getEnglish())) {
        wrongWords.push_back(word);
        
        // 添加到数据库
        QSqlQuery query;
        query.prepare("INSERT OR REPLACE INTO wrong_words "
                     "(username, word_english, added_date) "
                     "VALUES (?, ?, ?)");
                     
        query.addBindValue(QString::fromStdString(user.getUsername()));
        query.addBindValue(QString::fromStdString(word.getEnglish()));
        query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
        
        query.exec();
    }
}

void Statistics::removeFromWrongWords(const Word& word) {
    // 从内存中移除
    wrongWords.erase(
        std::remove_if(wrongWords.begin(), wrongWords.end(),
            [&word](const Word& w) { return w.getEnglish() == word.getEnglish(); }),
        wrongWords.end()
    );
    
    // 从数据库中移除
    QSqlQuery query;
    query.prepare("DELETE FROM wrong_words WHERE username = ? AND word_english = ?");
    query.addBindValue(QString::fromStdString(user.getUsername()));
    query.addBindValue(QString::fromStdString(word.getEnglish()));
    query.exec();
}

std::vector<Word> Statistics::getWrongWords() const {
    return wrongWords;
}

bool Statistics::isInWrongWords(const std::string& english) const {
    return std::any_of(wrongWords.begin(), wrongWords.end(),
        [&english](const Word& w) { return w.getEnglish() == english; });
}

std::vector<Word> Statistics::getRandomWrongWords(int count) {
    if (count >= wrongWords.size()) {
        return wrongWords;
    }
    
    // 复制错题列表并打乱
    std::vector<Word> randomWords = wrongWords;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(randomWords.begin(), randomWords.end(), gen);
    
    // 返回指定数量的错题
    randomWords.resize(count);
    return randomWords;
}

int Statistics::getWrongWordsCount() const {
    return wrongWords.size();
}

Statistics::DailyStatistics Statistics::getTodayStatistics() const {
    DailyStatistics stats;
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    
    QSqlQuery query;
    query.prepare("SELECT * FROM daily_statistics WHERE username = ? AND date = ?");
    query.addBindValue(QString::fromStdString(user.getUsername()));
    query.addBindValue(today);
    
    if (query.exec() && query.next()) {
        stats.date = today.toStdString();
        stats.score = query.value("score").toInt();
        stats.correctCount = query.value("correct_count").toInt();
        stats.wrongCount = query.value("wrong_count").toInt();
        stats.accuracy = stats.correctCount + stats.wrongCount > 0 
            ? static_cast<double>(stats.correctCount) / (stats.correctCount + stats.wrongCount)
            : 0.0;
    }
    
    return stats;
}

std::vector<Statistics::DailyStatistics> Statistics::getWeeklyStatistics() const {
    std::vector<DailyStatistics> weekStats;
    QDate today = QDate::currentDate();
    QDate weekAgo = today.addDays(-7);
    
    QSqlQuery query;
    query.prepare("SELECT * FROM daily_statistics WHERE username = ? "
                 "AND date >= ? AND date <= ? ORDER BY date DESC");
                 
    query.addBindValue(QString::fromStdString(user.getUsername()));
    query.addBindValue(weekAgo.toString("yyyy-MM-dd"));
    query.addBindValue(today.toString("yyyy-MM-dd"));
    
    if (query.exec()) {
        while (query.next()) {
            DailyStatistics stats;
            stats.date = query.value("date").toString().toStdString();
            stats.score = query.value("score").toInt();
            stats.correctCount = query.value("correct_count").toInt();
            stats.wrongCount = query.value("wrong_count").toInt();
            stats.accuracy = stats.correctCount + stats.wrongCount > 0 
                ? static_cast<double>(stats.correctCount) / (stats.correctCount + stats.wrongCount)
                : 0.0;
            weekStats.push_back(stats);
        }
    }
    
    return weekStats;
}

double Statistics::getOverallAccuracy() const {
    QSqlQuery query;
    query.prepare("SELECT SUM(correct_count) as total_correct, "
                 "SUM(wrong_count) as total_wrong "
                 "FROM daily_statistics WHERE username = ?");
                 
    query.addBindValue(QString::fromStdString(user.getUsername()));
    
    if (query.exec() && query.next()) {
        int totalCorrect = query.value("total_correct").toInt();
        int totalWrong = query.value("total_wrong").toInt();
        return totalCorrect + totalWrong > 0 
            ? static_cast<double>(totalCorrect) / (totalCorrect + totalWrong)
            : 0.0;
    }
    
    return 0.0;
}

bool Statistics::saveStatistics() {
    return saveWrongWordsToDatabase();
}

bool Statistics::loadStatistics() {
    return loadWrongWordsFromDatabase();
}

bool Statistics::saveWrongWordsToDatabase() {
    QSqlQuery query;
    query.prepare("DELETE FROM wrong_words WHERE username = ?");
    query.addBindValue(QString::fromStdString(user.getUsername()));
    if (!query.exec()) {
        return false;
    }
    
    for (const auto& word : wrongWords) {
        query.prepare("INSERT INTO wrong_words (username, word_english, added_date) "
                     "VALUES (?, ?, ?)");
        query.addBindValue(QString::fromStdString(user.getUsername()));
        query.addBindValue(QString::fromStdString(word.getEnglish()));
        query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
        
        if (!query.exec()) {
            return false;
        }
    }
    
    return true;
}

bool Statistics::loadWrongWordsFromDatabase() {
    wrongWords.clear();
    
    QSqlQuery query;
    query.prepare("SELECT w.* FROM wrong_words w "
                 "JOIN words ON w.word_english = words.english "
                 "WHERE w.username = ?");
    query.addBindValue(QString::fromStdString(user.getUsername()));
    
    if (query.exec()) {
        while (query.next()) {
            std::string english = query.value("word_english").toString().toStdString();
            // 从词库中获取完整的单词信息
            // 注意：这里假设有一个从词库获取单词的方法
            Word word = Word::getWordFromDatabase(english);
            if (!word.getEnglish().empty()) {
                wrongWords.push_back(word);
            }
        }
        return true;
    }
    
    return false;
}
