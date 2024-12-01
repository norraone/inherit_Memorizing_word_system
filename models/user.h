#ifndef USER_H
#define USER_H

#include <string>
#include <chrono>
#include <vector>
#include "word.h"

class User {
public:
    struct LearningStats {
        int totalScore = 0;
        int daysStreak = 0;
        int totalWordsLearned = 0;
        std::chrono::system_clock::time_point lastCheckinDate;
        
        bool hasCheckedInToday() const {
            auto now = std::chrono::system_clock::now();
            auto today = std::chrono::floor<std::chrono::days>(now);
            auto lastCheckin = std::chrono::floor<std::chrono::days>(lastCheckinDate);
            return today == lastCheckin;
        }
    };

private:
    std::string username;
    std::string passwordHash;  // Stored as SHA-256 hash
    LearningStats stats;
    std::chrono::system_clock::time_point createdAt;
    
public:
    User() = default;
    User(const std::string& username, const std::string& password);
    
    // Getters
    const std::string& getUsername() const { return username; }
    const LearningStats& getStats() const { return stats; }
    std::chrono::system_clock::time_point getCreatedAt() const { return createdAt; }
    
    // Learning methods
    void checkIn();
    void addScore(int points);
    void recordWordLearned();
    bool verifyPassword(const std::string& password) const;
    
    // Static methods
    static std::string hashPassword(const std::string& password);
    
    friend class UserRepository;  // Allow repository to access private members
};

#endif // USER_H
