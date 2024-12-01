#include "review_service.h"
#include <stdexcept>

void ReviewService::startNewSession(const std::string& username, int wordCount) {
    if (wordCount <= 0) {
        throw std::invalid_argument("Word count must be positive");
    }
    
    currentUser = username;
    auto dueWords = wordRepository->findDueForReview(username, wordCount);
    
    if (dueWords.empty()) {
        throw std::runtime_error("No words due for review");
    }
    
    currentSession = std::make_unique<ReviewSession>(dueWords);
}

void ReviewService::endSession() {
    if (!currentSession) {
        throw std::runtime_error("No active review session");
    }
    
    // Save review results to database
    for (const auto& item : currentSession->getItems()) {
        if (item.reviewed) {
            updateWordDifficulty(item.word.getEnglish(), item.correct);
        }
    }
    
    currentSession.reset();
}

bool ReviewService::hasNextWord() const {
    if (!currentSession) {
        return false;
    }
    return currentSession->hasNext();
}

const Word& ReviewService::getCurrentWord() const {
    if (!currentSession) {
        throw std::runtime_error("No active review session");
    }
    return currentSession->getCurrentItem().word;
}

void ReviewService::recordAttempt(bool correct) {
    if (!currentSession) {
        throw std::runtime_error("No active review session");
    }
    currentSession->recordAttempt(correct);
}

double ReviewService::getCurrentAccuracy() const {
    if (!currentSession) {
        throw std::runtime_error("No active review session");
    }
    return currentSession->getAccuracy();
}

std::chrono::seconds ReviewService::getSessionTime() const {
    if (!currentSession) {
        throw std::runtime_error("No active review session");
    }
    return currentSession->getElapsedTime();
}

int ReviewService::getCorrectCount() const {
    if (!currentSession) {
        throw std::runtime_error("No active review session");
    }
    return currentSession->getCorrectCount();
}

int ReviewService::getTotalCount() const {
    if (!currentSession) {
        throw std::runtime_error("No active review session");
    }
    return currentSession->getTotalCount();
}

std::vector<ReviewSession::ReviewItem> ReviewService::getReviewedItems() const {
    if (!currentSession) {
        throw std::runtime_error("No active review session");
    }
    return currentSession->getItems();
}

void ReviewService::updateWordDifficulty(const std::string& english, bool wasCorrect) {
    auto word = wordRepository->findByEnglish(english);
    if (!word) {
        throw std::runtime_error("Word not found");
    }
    
    word->recordAttempt(wasCorrect);
    wordRepository->update(*word);
}

std::vector<Word> ReviewService::getMostDifficultWords(int limit) {
    return wordRepository->getMostDifficultWords(limit);
}
