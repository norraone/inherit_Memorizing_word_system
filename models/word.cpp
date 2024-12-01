#include "word.h"

Word::Word(std::string eng, std::string pos, std::string chn)
    : english(std::move(eng)), partOfSpeech(std::move(pos)), chinese(std::move(chn)), addedDate(std::chrono::system_clock::now()) {}

void Word::recordAttempt(bool correct) {
    stats.totalAttempts++;
    if (correct) {
        stats.correctCount++;
    }
    stats.frequency++;
}

void Word::addDefinition(const std::string& type, const std::string& content) {
    Definition def{type, content, std::chrono::system_clock::now()};
    definitions.push_back(def);
}

void Word::addCategory(const std::string& category) {
    if (std::find(categories.begin(), categories.end(), category) == categories.end()) {
        categories.push_back(category);
    }
}

void Word::removeCategory(const std::string& category) {
    categories.erase(
        std::remove(categories.begin(), categories.end(), category),
        categories.end()
    );
}
