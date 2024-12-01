#ifndef REVIEW_SESSION_H
#define REVIEW_SESSION_H

#include "word.h"
#include <vector>
#include <chrono>
#include <random>

class ReviewSession {
public:
    struct ReviewItem {
        Word word;
        int masteryLevel;  // 1-4: beginner, familiar, mastered, expert
        std::chrono::system_clock::time_point nextReviewDate;
        bool reviewed;     // Whether this item has been reviewed in current session
        bool correct;      // Whether the last review was correct
        
        ReviewItem(const Word& w, int level = 1)
            : word(w), masteryLevel(level), reviewed(false), correct(false) {
            updateNextReviewDate();
        }
        
        void updateNextReviewDate() {
            auto now = std::chrono::system_clock::now();
            // Spaced repetition intervals: 1 day, 3 days, 7 days, 14 days
            int daysToAdd = 0;
            switch (masteryLevel) {
                case 1: daysToAdd = 1; break;
                case 2: daysToAdd = 3; break;
                case 3: daysToAdd = 7; break;
                default: daysToAdd = 14; break;
            }
            nextReviewDate = now + std::chrono::hours(24 * daysToAdd);
        }
    };

private:
    std::vector<ReviewItem> items;
    size_t currentIndex;
    std::mt19937 rng;
    int correctCount;
    int totalCount;
    std::chrono::system_clock::time_point startTime;

public:
    ReviewSession(const std::vector<Word>& words);
    
    // Session control
    bool hasNext() const { return currentIndex < items.size(); }
    const ReviewItem& getCurrentItem() const { return items[currentIndex]; }
    void recordAttempt(bool correct);
    void shuffle();
    
    // Statistics
    double getAccuracy() const;
    std::chrono::seconds getElapsedTime() const;
    int getCorrectCount() const { return correctCount; }
    int getTotalCount() const { return totalCount; }
    const std::vector<ReviewItem>& getItems() const { return items; }
};

#endif // REVIEW_SESSION_H
