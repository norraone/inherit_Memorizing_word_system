#ifndef WORD_SERVICE_H
#define WORD_SERVICE_H

#include "../repositories/word_repository.h"
#include "../models/word.h"
#include <memory>
#include <vector>
#include <optional>

class WordService {
private:
    std::unique_ptr<WordRepository> repository;

public:
    WordService() : repository(std::make_unique<WordRepository>()) {}

    // Core word operations
    std::optional<Word> getWord(const std::string& english);
    bool addWord(const Word& word);
    bool updateWord(const Word& word);
    bool deleteWord(const std::string& english);

    // Learning operations
    std::vector<Word> getWordsForReview(const std::string& username, int count = 10);
    void recordWordAttempt(const std::string& english, bool correct);
    
    // Statistics
    double getUserAccuracy(const std::string& username);
    std::vector<Word> getDifficultWords(int limit = 10);
    int getLearnedWordsCount(const std::string& username);
};

#endif // WORD_SERVICE_H
