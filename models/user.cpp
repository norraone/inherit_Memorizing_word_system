#include "user.h"
#include <QCryptographicHash>
#include <QString>

User::User(const std::string& username, const std::string& password)
    : username(username),
      passwordHash(hashPassword(password)),
      createdAt(std::chrono::system_clock::now()) {}

void User::checkIn() {
    auto now = std::chrono::system_clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    auto last_t = std::chrono::system_clock::to_time_t(stats.lastCheckinDate);
    
    // Convert to struct tm to compare dates
    struct tm now_tm = *localtime(&now_t);
    struct tm last_tm = *localtime(&last_t);
    
    // Calculate days difference using mktime
    now_tm.tm_hour = 0;
    now_tm.tm_min = 0;
    now_tm.tm_sec = 0;
    last_tm.tm_hour = 0;
    last_tm.tm_min = 0;
    last_tm.tm_sec = 0;
    
    double diff_seconds = difftime(mktime(&now_tm), mktime(&last_tm));
    int days_diff = static_cast<int>(diff_seconds / (60 * 60 * 24));
    
    if (days_diff > 0) {
        if (days_diff == 1) {
            // Consecutive day
            stats.daysStreak++;
        } else {
            // Streak broken
            stats.daysStreak = 1;
        }
        stats.lastCheckinDate = now;
    }
}

void User::addScore(int points) {
    if (points > 0) {
        stats.totalScore += points;
    }
}

void User::recordWordLearned() {
    stats.totalWordsLearned++;
}

bool User::verifyPassword(const std::string& password) const {
    return passwordHash == hashPassword(password);
}

std::string User::hashPassword(const std::string& password) {
    QByteArray data = QByteArray::fromStdString(password);
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return hash.toHex().toStdString();
}

void User::updatePassword(const std::string& newPassword) {
    passwordHash = hashPassword(newPassword);
}
