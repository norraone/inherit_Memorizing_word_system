#ifndef WORD_REPOSITORY_H
#define WORD_REPOSITORY_H

#include "base_repository.h"
#include "../models/word.h"
#include <vector>
#include <optional>
#include <map>
#include <QSqlQuery>

class WordRepository : public BaseRepository {
public:
    std::optional<Word> findByEnglish(const std::string& english);
    std::vector<Word> findByCategory(const std::string& category);
    std::vector<Word> findDueForReview(const std::string& username, int limit = 10);
    bool save(const Word& word);
    bool update(const Word& word);
    bool remove(const std::string& english);
    
    // Statistics
    int getTotalWordCount();
    std::vector<Word> getMostDifficultWords(int limit = 10);
    std::vector<Word> getMostFrequentWords(int limit = 10);
    
    // New statistics methods
    struct WordStats {
        std::string english;
        int attempts;
        int correctCount;
        double accuracy;
        int frequency;
    };
    
    struct DailyStats {
        std::chrono::system_clock::time_point date;
        int wordsLearned;
        int wordsReviewed;
        double accuracy;
    };
    
    std::vector<WordStats> getWordStats(const std::string& username);
    std::vector<DailyStats> getDailyStats(const std::string& username, const std::chrono::system_clock::time_point& date);
    std::map<std::string, int> getWordCountByCategory();
    std::vector<WordStats> getMostReviewedWords(int limit = 10);
    int getTotalReviewTime(const std::string& username);
    
    // Add this method to retrieve all words
    std::vector<Word> getAllWords();
};

#endif // WORD_REPOSITORY_H
