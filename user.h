#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <QSqlDatabase>
#include "database.h"

class User {
private:
    static QSqlDatabase db;
    static bool dbInitialized;
    
    std::string username;
    std::string password;  // 存储哈希后的密码
    int totalScore;
    int daysStreak;
    int totalWordsLearned;
    std::string lastCheckInDate;
    bool isLoggedIn;
    
    // 数据库初始化
    static void initializeDatabase();
    
    // 密码哈希
    static std::string hashPassword(const std::string& pwd);
    
    // 验证密码
    bool verifyPassword(const std::string& inputPwd) const;
    
    // 检查用户名是否存在
    static bool usernameExists(const std::string& uname);
    
    // 加载用户数据
    bool loadUserData();
    
    // 学习记录结构
    struct LearningRecord {
        std::string date;
        std::string word;
        bool correct;
    };
    
    // 打卡记录结构
    struct CheckInRecord {
        std::string date;
        bool withLearning;
        int streakCount;
    };

public:
    User();
    User(const std::string& uname, const std::string& pwd);
    ~User();
    
    // 用户认证
    struct RegistrationResult {
        bool success;
        std::string message;
    };
    
    struct LoginResult {
        bool success;
        std::string message;
        User* user;
    };
    
    // 静态方法：注册新用户
    static RegistrationResult registerUser(const std::string& username, 
                                         const std::string& password,
                                         const std::string& confirmPassword);
    
    // 静态方法：用户登录
    static LoginResult login(const std::string& username, 
                           const std::string& password);
    
    // 登出
    void logout();
    
    // 修改密码
    bool changePassword(const std::string& oldPassword, 
                       const std::string& newPassword);
    
    // 检查登录状态
    bool isAuthenticated() const { return isLoggedIn; }
    
    // 基本信息获取
    std::string getUsername() const { return username; }
    int getTotalScore() const { return totalScore; }
    int getDaysStreak() const { return daysStreak; }
    int getTotalWordsLearned() const { return totalWordsLearned; }
    std::string getLastCheckInDate() const { return lastCheckInDate; }
    
    // 学习记录更新
    void updateScore(int points);
    void updateLearningRecord(const std::string& word, bool correct);
    void incrementTotalWordsLearned();
    void incrementWordsLearned();
    bool addLearningRecord(const LearningRecord& record);
    std::vector<LearningRecord> getLearningHistory(int days) const;
    
    // 打卡相关
    bool checkIn();
    bool hasCheckedInToday() const;
    int getMissedDays() const;
    bool needResetStreak() const;
    void resetDaysStreak() { daysStreak = 0; }
    void incrementDaysStreak() { ++daysStreak; }
    bool addCheckInRecord(bool withLearning);
    std::vector<CheckInRecord> getCheckInHistory(int days) const;
    int getMonthlyCheckInCount() const;
    
    // 数据库操作
    bool saveToDatabase();
    bool loadFromDatabase();
    
    // 获取学习统计
    int getTotalAttempts() const;
    int getCorrectAnswers() const;
    double getLearningAccuracy() const;
    int getLearningTime() const;  // 返回分钟数
    
    // 密码验证规则
    static bool isValidPassword(const std::string& password);
    static bool isValidUsername(const std::string& username);
};

#endif // USER_H
