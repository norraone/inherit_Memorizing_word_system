#include "statistics_service.h"
#include <algorithm>
#include <numeric>

StatisticsService::StatisticsService()
    : wordRepository(std::make_unique<WordRepository>()),
      userRepository(std::make_unique<UserRepository>()) {}

StatisticsService::UserProgress StatisticsService::getUserProgress(const std::string& username) {
    UserProgress progress{};
    auto user = userRepository->findByUsername(username);
    if (!user) return progress;
    
    auto words = wordRepository->getWordStats(username);
    progress.totalWords = words.size();
    progress.totalScore = user->getStats().totalScore;
    progress.daysStreak = user->getStats().daysStreak;
    
    int totalAttempts = 0;
    int totalCorrect = 0;
    
    for (const auto& word : words) {
        if (word.attempts == 0) {
            progress.newWords++;
        } else {
            double accuracy = static_cast<double>(word.correctCount) / word.attempts;
            if (accuracy >= 0.8) {
                progress.masteredWords++;
            } else {
                progress.learningWords++;
            }
            totalAttempts += word.attempts;
            totalCorrect += word.correctCount;
        }
    }
    
    progress.overallAccuracy = totalAttempts > 0 
        ? static_cast<double>(totalCorrect) / totalAttempts 
        : 0.0;
    
    return progress;
}

std::vector<StatisticsService::DailyStats> StatisticsService::getDailyStats(
    const std::string& username, int days) {
    std::vector<DailyStats> stats;
    auto now = std::chrono::system_clock::now();
    
    for (int i = 0; i < days; ++i) {
        auto date = now - std::chrono::hours(24 * i);
        auto dayStats = wordRepository->getDailyStats(username, date);
        
        DailyStats ds;
        ds.date = date;
        ds.wordsLearned = dayStats.wordsLearned;
        ds.wordsReviewed = dayStats.wordsReviewed;
        ds.correctCount = dayStats.correctCount;
        ds.accuracy = dayStats.wordsReviewed > 0 
            ? static_cast<double>(dayStats.correctCount) / dayStats.wordsReviewed 
            : 0.0;
            
        stats.push_back(ds);
    }
    
    return stats;
}

std::vector<StatisticsService::WordStats> StatisticsService::getWordStats(
    const std::string& username) {
    std::vector<WordStats> stats;
    auto words = wordRepository->getWordStats(username);
    
    for (const auto& word : words) {
        WordStats ws;
        ws.english = word.english;
        ws.chinese = word.chinese;
        ws.attempts = word.attempts;
        ws.correctCount = word.correctCount;
        ws.accuracy = word.attempts > 0 
            ? static_cast<double>(word.correctCount) / word.attempts 
            : 0.0;
        ws.lastReview = word.lastReview;
        
        stats.push_back(ws);
    }
    
    return stats;
}

std::map<std::string, int> StatisticsService::getWordsByCategory() {
    return wordRepository->getWordCountByCategory();
}

std::vector<StatisticsService::WordStats> StatisticsService::getMostDifficultWords(int limit) {
    std::vector<WordStats> stats;
    auto words = wordRepository->getMostDifficultWords(limit);
    
    for (const auto& word : words) {
        WordStats ws;
        ws.english = word.getEnglish();
        ws.chinese = word.getChinese();
        ws.attempts = word.getStats().totalAttempts;
        ws.correctCount = word.getStats().correctCount;
        ws.accuracy = word.getStats().getAccuracy();
        
        stats.push_back(ws);
    }
    
    return stats;
}

std::vector<StatisticsService::WordStats> StatisticsService::getMostReviewedWords(int limit) {
    std::vector<WordStats> stats;
    auto words = wordRepository->getMostReviewedWords(limit);
    
    for (const auto& word : words) {
        WordStats ws;
        ws.english = word.getEnglish();
        ws.chinese = word.getChinese();
        ws.attempts = word.getStats().totalAttempts;
        ws.correctCount = word.getStats().correctCount;
        ws.accuracy = word.getStats().getAccuracy();
        
        stats.push_back(ws);
    }
    
    return stats;
}

int StatisticsService::getLongestStreak(const std::string& username) {
    auto user = userRepository->findByUsername(username);
    return user ? user->getStats().daysStreak : 0;
}

int StatisticsService::getTotalReviewTime(const std::string& username) {
    return wordRepository->getTotalReviewTime(username);
}

double StatisticsService::getAverageAccuracy(const std::string& username) {
    auto stats = getWordStats(username);
    if (stats.empty()) return 0.0;
    
    double totalAccuracy = std::accumulate(stats.begin(), stats.end(), 0.0,
        [](double sum, const WordStats& ws) { return sum + ws.accuracy; });
    
    return totalAccuracy / stats.size();
}
