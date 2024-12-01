#include "review_session.h"
#include <algorithm>

ReviewSession::ReviewSession(const std::vector<Word>& words)
    : currentIndex(0), correctCount(0), totalCount(0),
      startTime(std::chrono::system_clock::now()) {
    // Initialize random number generator with random device
    std::random_device rd;
    rng.seed(rd());
    
    // Create review items from words
    items.reserve(words.size());
    for (const auto& word : words) {
        items.emplace_back(word);
    }
    
    shuffle();
}

void ReviewSession::recordAttempt(bool correct) {
    if (currentIndex >= items.size()) return;
    
    auto& item = items[currentIndex];
    item.reviewed = true;
    item.correct = correct;
    
    // Update mastery level based on correctness
    if (correct) {
        if (item.masteryLevel < 4) {
            item.masteryLevel++;
        }
        correctCount++;
    } else {
        if (item.masteryLevel > 1) {
            item.masteryLevel--;
        }
    }
    
    item.updateNextReviewDate();
    totalCount++;
    currentIndex++;
}

void ReviewSession::shuffle() {
    std::shuffle(items.begin(), items.end(), rng);
    currentIndex = 0;
}

double ReviewSession::getAccuracy() const {
    return totalCount > 0 ? static_cast<double>(correctCount) / totalCount : 0.0;
}

std::chrono::seconds ReviewSession::getElapsedTime() const {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - startTime);
}
