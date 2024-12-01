#include "user.h"
#include <QCryptographicHash>
#include <QString>

User::User(const std::string& username, const std::string& password)
    : username(username),
      passwordHash(hashPassword(password)),
      createdAt(std::chrono::system_clock::now()) {}

void User::checkIn() {
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::floor<std::chrono::days>(now);
    auto lastCheckin = std::chrono::floor<std::chrono::days>(stats.lastCheckinDate);
    
    if (today > lastCheckin) {
        if (today - lastCheckin == std::chrono::days(1)) {
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
