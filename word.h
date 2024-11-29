#ifndef WORD_H
#define WORD_H

#include <string>

class Word {
private:
    std::string english;
    std::string partOfSpeech;
    std::string chinese;
    int frequency;
    int correctCount;
    int totalAttempts;

public:
    Word();
    Word(const std::string& eng, const std::string& pos, const std::string& chn);
    
    // Getters
    std::string getEnglish() const;
    std::string getPartOfSpeech() const;
    std::string getChinese() const;
    double getCorrectRate() const;
    int getFrequency() const;
    int getCorrectCount() const;
    int getTotalAttempts() const;
    
    // Setters
    void setEnglish(const std::string& eng);
    void setPartOfSpeech(const std::string& pos);
    void setChinese(const std::string& chn);
    void setFrequency(int freq);
    void setCorrectCount(int count);
    void setTotalAttempts(int attempts);
    
    // 学习相关方法
    void incrementFrequency();
    void recordAttempt(bool correct);
    bool checkSpelling(const std::string& attempt) const;
};

#endif // WORD_H
