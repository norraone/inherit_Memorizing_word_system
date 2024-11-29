#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string username;
    std::string password;
    int totalScore;
    int daysStreak;
    int totalWordsLearned;

public:
    User();
    User(const std::string& username, const std::string& password);
    
    // Getters
    std::string getUsername() const;
    int getTotalScore() const;
    int getDaysStreak() const;
    int getTotalWordsLearned() const;
    
    // Setters
    void setUsername(const std::string& username);
    void setPassword(const std::string& password);
    void updateScore(int points);
    void incrementDaysStreak();
    void incrementWordsLearned();
    
    // Authentication
    bool checkPassword(const std::string& inputPassword) const;
    
    // Statistics
    void saveProgress();
    void loadProgress();
};

#endif // USER_H
