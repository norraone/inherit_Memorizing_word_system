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
            // Convert to time_t for easier day comparison
            auto now_t = std::chrono::system_clock::to_time_t(now);
            auto last_t = std::chrono::system_clock::to_time_t(lastCheckinDate);
            
            // Convert to struct tm to compare year/month/day
            struct tm now_tm = *localtime(&now_t);
            struct tm last_tm = *localtime(&last_t);
            
            return (now_tm.tm_year == last_tm.tm_year &&
                    now_tm.tm_mon == last_tm.tm_mon &&
                    now_tm.tm_mday == last_tm.tm_mday);
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
    
    // Authentication methods
    bool verifyPassword(const std::string& password) const;
    void updatePassword(const std::string& newPassword);
    
    // Password hashing
    static std::string hashPassword(const std::string& password);
    
    // Static methods
    
    friend class UserRepository;  // Allow repository to access private members
};

#endif // USER_H
