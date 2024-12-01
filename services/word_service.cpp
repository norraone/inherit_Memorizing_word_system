#include "word_service.h"
#include <stdexcept>

std::optional<Word> WordService::getWord(const std::string& english) {
    return repository->findByEnglish(english);
}

bool WordService::addWord(const Word& word) {
    // Validate word data
    if (word.getEnglish().empty() || word.getChinese().empty()) {
        throw std::invalid_argument("Word must have both English and Chinese translations");
    }
    
    // Check if word already exists
    if (repository->findByEnglish(word.getEnglish())) {
        throw std::runtime_error("Word already exists");
    }
    
    return repository->save(word);
}

bool WordService::updateWord(const Word& word) {
    // Validate word data
    if (word.getEnglish().empty() || word.getChinese().empty()) {
        throw std::invalid_argument("Word must have both English and Chinese translations");
    }
    
    // Check if word exists
    if (!repository->findByEnglish(word.getEnglish())) {
        throw std::runtime_error("Word does not exist");
    }
    
    return repository->update(word);
}

bool WordService::deleteWord(const std::string& english) {
    // Check if word exists
    if (!repository->findByEnglish(english)) {
        throw std::runtime_error("Word does not exist");
    }
    
    return repository->remove(english);
}

std::vector<Word> WordService::getWordsForReview(const std::string& username, int count) {
    if (username.empty()) {
        throw std::invalid_argument("Username cannot be empty");
    }
    
    if (count <= 0) {
        throw std::invalid_argument("Count must be positive");
    }
    
    return repository->findDueForReview(username, count);
}

void WordService::recordWordAttempt(const std::string& english, bool correct) {
    auto wordOpt = repository->findByEnglish(english);
    if (!wordOpt) {
        throw std::runtime_error("Word does not exist");
    }
    
    Word word = *wordOpt;
    word.recordAttempt(correct);
    repository->update(word);
}

std::vector<Word> WordService::getDifficultWords(int limit) {
    if (limit <= 0) {
        throw std::invalid_argument("Limit must be positive");
    }
    
    return repository->getMostDifficultWords(limit);
}

int WordService::getLearnedWordsCount(const std::string& username) {
    if (username.empty()) {
        throw std::invalid_argument("Username cannot be empty");
    }
    
    // TODO: Implement this in repository
    return 0;
}
