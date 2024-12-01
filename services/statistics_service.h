#ifndef STATISTICS_SERVICE_H
#define STATISTICS_SERVICE_H

#include "../repositories/word_repository.h"
#include "../repositories/user_repository.h"
#include <memory>
#include <vector>
#include <map>
#include <chrono>

class StatisticsService {
public:
    struct DailyStats {
        int wordsLearned;
        int wordsReviewed;
        int correctCount;
        double accuracy;
        std::chrono::system_clock::time_point date;
    };
    
    struct WordStats {
        std::string english;
        std::string chinese;
        int attempts;
        int correctCount;
        double accuracy;
        std::chrono::system_clock::time_point lastReview;
    };
    
    struct UserProgress {
        int totalWords;
        int masteredWords;  // Words with accuracy > 80%
        int learningWords;  // Words in progress
        int newWords;       // Words not started
        double overallAccuracy;
        int daysStreak;
        int totalScore;
    };

private:
    std::unique_ptr<WordRepository> wordRepository;
    std::unique_ptr<UserRepository> userRepository;
    std::string currentUser;

public:
    StatisticsService();
    
    // User progress
    UserProgress getUserProgress(const std::string& username);
    std::vector<DailyStats> getDailyStats(const std::string& username, int days = 7);
    std::vector<WordStats> getWordStats(const std::string& username);
    
    // Learning analytics
    std::map<std::string, int> getWordsByCategory();
    std::vector<WordStats> getMostDifficultWords(int limit = 10);
    std::vector<WordStats> getMostReviewedWords(int limit = 10);
    
    // Achievement tracking
    int getLongestStreak(const std::string& username);
    int getTotalReviewTime(const std::string& username);  // in minutes
    double getAverageAccuracy(const std::string& username);
};

#endif // STATISTICS_SERVICE_H
