#ifndef WORD_REPOSITORY_H
#define WORD_REPOSITORY_H

#include "base_repository.h"
#include "../models/word.h"
#include <vector>
#include <optional>
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
};

#endif // WORD_REPOSITORY_H
