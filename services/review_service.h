#ifndef REVIEW_SERVICE_H
#define REVIEW_SERVICE_H

#include "../models/review_session.h"
#include "../repositories/word_repository.h"
#include <memory>

class ReviewService {
private:
    std::unique_ptr<WordRepository> wordRepository;
    std::unique_ptr<ReviewSession> currentSession;
    std::string currentUser;

public:
    ReviewService() : wordRepository(std::make_unique<WordRepository>()) {}
    
    // Session management
    void startNewSession(const std::string& username, int wordCount = 10);
    void endSession();
    bool hasActiveSession() const { return currentSession != nullptr; }
    
    // Review operations
    bool hasNextWord() const;
    const Word& getCurrentWord() const;
    void recordAttempt(bool correct);
    
    // Statistics
    double getCurrentAccuracy() const;
    std::chrono::seconds getSessionTime() const;
    int getCorrectCount() const;
    int getTotalCount() const;
    std::vector<ReviewSession::ReviewItem> getReviewedItems() const;
    
    // Word difficulty tracking
    void updateWordDifficulty(const std::string& english, bool wasCorrect);
    std::vector<Word> getMostDifficultWords(int limit = 10);
};

#endif // REVIEW_SERVICE_H
