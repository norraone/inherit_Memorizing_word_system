#include "word.h"

Word::Word() : frequency(0), correctCount(0), totalAttempts(0) {}

Word::Word(const std::string& eng, const std::string& pos, const std::string& chn)
    : english(eng), partOfSpeech(pos), chinese(chn), frequency(0), correctCount(0), totalAttempts(0) {}

// Getters
std::string Word::getEnglish() const { return english; }
std::string Word::getPartOfSpeech() const { return partOfSpeech; }
std::string Word::getChinese() const { return chinese; }
double Word::getCorrectRate() const {
    return totalAttempts > 0 ? static_cast<double>(correctCount) / totalAttempts : 0.0;
}
int Word::getFrequency() const { return frequency; }
int Word::getCorrectCount() const { return correctCount; }
int Word::getTotalAttempts() const { return totalAttempts; }

// Setters
void Word::setEnglish(const std::string& eng) { english = eng; }
void Word::setPartOfSpeech(const std::string& pos) { partOfSpeech = pos; }
void Word::setChinese(const std::string& chn) { chinese = chn; }
void Word::setFrequency(int freq) { frequency = freq; }
void Word::setCorrectCount(int count) { correctCount = count; }
void Word::setTotalAttempts(int attempts) { totalAttempts = attempts; }

// 学习相关方法
void Word::incrementFrequency() { frequency++; }

void Word::recordAttempt(bool correct) {
    totalAttempts++;
    if (correct) correctCount++;
}

bool Word::checkSpelling(const std::string& attempt) const {
    return attempt == english;
}
