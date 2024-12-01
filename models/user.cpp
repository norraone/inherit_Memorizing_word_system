#include "user.h"
#include <QCryptographicHash>
#include <QString>

User::User(const std::string& username, const std::string& password)
    : username(username),
      passwordHash(hashPassword(password)),
      createdAt(std::chrono::system_clock::now()) {
    // Initialize stats with default values
    stats.totalScore = 0;
    stats.daysStreak = 1;  // First check-in starts the streak
    stats.totalWordsLearned = 0;
    stats.lastCheckinDate = std::chrono::system_clock::time_point(); // Explicitly set to default
}

void User::checkIn() {
    auto now = std::chrono::system_clock::now();
    
    // If already checked in today, throw an exception
    if (stats.hasCheckedInToday()) {
        throw std::runtime_error("今天已经签到过了");
    }
    
    // Calculate streak
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
    
    // Update streak logic
    if (days_diff == 1) {
        // Consecutive day - increase streak
        stats.daysStreak++;
    } else if (days_diff > 1) {
        // Streak broken - reset to 1
        stats.daysStreak = 1;
    }
    
    // Update last check-in date
    stats.lastCheckinDate = now;
    
    // Award points based on streak
    int points = calculateCheckInPoints();
    addScore(points);
}

int User::calculateCheckInPoints() const {
    // Bonus points for consecutive days
    int basePoints = 10;  // Base points for check-in
    int streakBonus = stats.daysStreak * 2;  // 2 bonus points per consecutive day
    
    // Cap the bonus to prevent excessive point accumulation
    const int MAX_STREAK_BONUS = 50;
    streakBonus = std::min(streakBonus, MAX_STREAK_BONUS);
    
    // Special milestone bonuses
    if (stats.daysStreak == 7) {
        streakBonus += 50;  // Week-long streak bonus
    } else if (stats.daysStreak == 30) {
        streakBonus += 200;  // Month-long streak bonus
    }
    
    return basePoints + streakBonus;
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
