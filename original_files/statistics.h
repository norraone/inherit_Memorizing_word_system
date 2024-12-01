#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <string>
#include <memory>
#include "word.h"
#include "user.h"

class Statistics {
private:
    static const int CORRECT_SCORE = 10;    // 答对加10分
    static const int WRONG_SCORE = -10;     // 答错扣10分
    
    User& user;
    std::vector<Word> wrongWords;           // 错题本
    int totalScore;                         // 总分
    int todayScore;                         // 今日得分
    
    // 数据库相关
    static void initializeDatabase();
    bool saveWrongWordsToDatabase();
    bool loadWrongWordsFromDatabase();
    
public:
    explicit Statistics(User& u);
    ~Statistics();
    
    // 分数管理
    void updateScore(bool isCorrect);                    // 更新分数
    int getTotalScore() const { return totalScore; }     // 获取总分
    int getTodayScore() const { return todayScore; }     // 获取今日得分
    
    // 错题本管理
    void addToWrongWords(const Word& word);              // 添加错题
    void removeFromWrongWords(const Word& word);         // 移除错题
    std::vector<Word> getWrongWords() const;             // 获取所有错题
    bool isInWrongWords(const std::string& english) const;  // 检查是否在错题本中
    void clearWrongWords();                              // 清空错题本
    
    // 错题复习
    std::vector<Word> getRandomWrongWords(int count);    // 获取随机错题
    int getWrongWordsCount() const;                      // 获取错题数量
    
    // 统计信息
    struct DailyStatistics {
        std::string date;
        int score;
        int correctCount;
        int wrongCount;
        double accuracy;
    };
    
    DailyStatistics getTodayStatistics() const;          // 获取今日统计
    std::vector<DailyStatistics> getWeeklyStatistics() const;  // 获取周统计
    double getOverallAccuracy() const;                   // 获取总体正确率
    
    // 保存和加载
    bool saveStatistics();                               // 保存统计信息
    bool loadStatistics();                               // 加载统计信息
};

#endif // STATISTICS_H
