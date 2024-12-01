#include "user_repository.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

std::optional<User> UserRepository::findByUsername(const std::string& username) {
    QSqlQuery query(db);
    query.prepare("SELECT * FROM users WHERE username = ?");
    query.addBindValue(QString::fromStdString(username));
    
    if (query.exec() && query.next()) {
        User user;
        user.username = query.value("username").toString().toStdString();
        user.passwordHash = query.value("password").toString().toStdString();
        user.stats.totalScore = query.value("total_score").toInt();
        user.stats.daysStreak = query.value("days_streak").toInt();
        user.stats.totalWordsLearned = query.value("total_words_learned").toInt();
        user.stats.lastCheckinDate = std::chrono::system_clock::from_time_t(
            QDateTime::fromString(query.value("last_checkin_date").toString(),
                                Qt::ISODate).toSecsSinceEpoch());
        user.createdAt = std::chrono::system_clock::from_time_t(
            QDateTime::fromString(query.value("created_at").toString(),
                                Qt::ISODate).toSecsSinceEpoch());
        return user;
    }
    
    return std::nullopt;
}

bool UserRepository::save(const User& user) {
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO users (username, password, total_score, days_streak, "
        "total_words_learned, last_checkin_date, created_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)"
    );
    
    query.addBindValue(QString::fromStdString(user.username));
    query.addBindValue(QString::fromStdString(user.passwordHash));
    query.addBindValue(user.stats.totalScore);
    query.addBindValue(user.stats.daysStreak);
    query.addBindValue(user.stats.totalWordsLearned);
    query.addBindValue(QDateTime::fromSecsSinceEpoch(
        std::chrono::system_clock::to_time_t(user.stats.lastCheckinDate))
        .toString(Qt::ISODate));
    query.addBindValue(QDateTime::fromSecsSinceEpoch(
        std::chrono::system_clock::to_time_t(user.createdAt))
        .toString(Qt::ISODate));
    
    return query.exec();
}

bool UserRepository::update(const User& user) {
    QSqlQuery query(db);
    query.prepare(
        "UPDATE users SET password = ?, total_score = ?, days_streak = ?, "
        "total_words_learned = ?, last_checkin_date = ? "
        "WHERE username = ?"
    );
    
    query.addBindValue(QString::fromStdString(user.passwordHash));
    query.addBindValue(user.stats.totalScore);
    query.addBindValue(user.stats.daysStreak);
    query.addBindValue(user.stats.totalWordsLearned);
    query.addBindValue(QDateTime::fromSecsSinceEpoch(
        std::chrono::system_clock::to_time_t(user.stats.lastCheckinDate))
        .toString(Qt::ISODate));
    query.addBindValue(QString::fromStdString(user.username));
    
    return query.exec();
}

bool UserRepository::remove(const std::string& username) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM users WHERE username = ?");
    query.addBindValue(QString::fromStdString(username));
    return query.exec();
}

std::vector<User> UserRepository::getTopUsers(int limit) {
    std::vector<User> users;
    QSqlQuery query(db);
    query.prepare(
        "SELECT * FROM users ORDER BY total_score DESC LIMIT ?"
    );
    query.addBindValue(limit);
    
    if (query.exec()) {
        while (query.next()) {
            auto user = findByUsername(
                query.value("username").toString().toStdString());
            if (user) {
                users.push_back(*user);
            }
        }
    }
    
    return users;
}

double UserRepository::getAverageWordsPerUser() {
    QSqlQuery query(db);
    query.exec("SELECT AVG(total_words_learned) FROM users");
    
    if (query.next()) {
        return query.value(0).toDouble();
    }
    
    return 0.0;
}

std::vector<User> UserRepository::getUsersByStreak(int minStreak) {
    std::vector<User> users;
    QSqlQuery query(db);
    query.prepare("SELECT * FROM users WHERE days_streak >= ?");
    query.addBindValue(minStreak);
    
    if (query.exec()) {
        while (query.next()) {
            auto user = findByUsername(
                query.value("username").toString().toStdString());
            if (user) {
                users.push_back(*user);
            }
        }
    }
    
    return users;
}
