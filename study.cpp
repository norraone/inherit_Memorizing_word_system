#include "study.h"
#include <algorithm>
#include <chrono>
#include <cctype>
#include <random>

Study::Study(User& u, Vocabulary& v) 
    : user(u), vocabulary(v), gen(rd()),
      currentDirection(TranslationDirection::ENGLISH_TO_CHINESE) {
}

void Study::startNewSession(int wordCount, TranslationDirection direction) {
    currentDirection = direction;
    // 获取单词列表
    currentWordList = vocabulary.getRandomWords(wordCount);
    // 打乱单词顺序
    std::shuffle(currentWordList.begin(), currentWordList.end(), gen);
}

void Study::setTranslationDirection(TranslationDirection direction) {
    currentDirection = direction;
}

TranslationDirection Study::getTranslationDirection() const {
    return currentDirection;
}

std::string Study::getQuestionText(const Word& word) const {
    return (currentDirection == TranslationDirection::ENGLISH_TO_CHINESE) 
           ? word.getEnglish() 
           : word.getChinese();
}

std::string Study::getAnswerText(const Word& word) const {
    return (currentDirection == TranslationDirection::ENGLISH_TO_CHINESE) 
           ? word.getChinese() 
           : word.getEnglish();
}

std::vector<std::string> Study::generateOptions(const Word& correctWord, TranslationDirection direction) {
    std::vector<std::string> options;
    // 添加正确答案
    options.push_back(getAnswerText(correctWord));
    
    // 获取3个干扰选项
    auto distractors = vocabulary.getRandomWords(3);
    for (const auto& word : distractors) {
        if (word.getEnglish() != correctWord.getEnglish()) {
            options.push_back(getAnswerText(word));
        }
    }
    
    // 如果没有足够的干扰选项，添加一些固定的选项
    while (options.size() < 4) {
        options.push_back(direction == TranslationDirection::ENGLISH_TO_CHINESE 
                         ? "未知选项" 
                         : "unknown");
    }
    
    return options;
}

void Study::shuffleOptions(std::vector<std::string>& options) {
    std::shuffle(options.begin(), options.end(), gen);
}

Study::MultipleChoiceQuestion Study::getNextMultipleChoice() {
    if (currentWordList.empty()) {
        return MultipleChoiceQuestion{};
    }
    
    Word currentWord = currentWordList.back();
    currentWordList.pop_back();
    
    // 生成选项
    auto options = generateOptions(currentWord, currentDirection);
    
    // 记录正确答案的位置
    int correctIndex = 0;
    std::string correctAnswer = getAnswerText(currentWord);
    
    // 打乱选项顺序
    shuffleOptions(options);
    
    // 找到正确答案的新位置
    for (size_t i = 0; i < options.size(); ++i) {
        if (options[i] == correctAnswer) {
            correctIndex = i;
            break;
        }
    }
    
    return MultipleChoiceQuestion{
        currentWord,
        getQuestionText(currentWord),
        options,
        correctIndex
    };
}

bool Study::checkMultipleChoiceAnswer(const MultipleChoiceQuestion& question, int selectedIndex) {
    bool isCorrect = (selectedIndex == question.correctIndex);
    
    // 更新单词的学习统计
    if (isCorrect) {
        question.word.incrementCorrectCount();
    } else {
        vocabulary.addToWrongWords(question.word);
    }
    
    // 更新用户学习记录
    user.updateLearningRecord(question.word.getEnglish(), isCorrect);
    
    return isCorrect;
}

Study::SpellingQuestion Study::getNextSpellingQuestion() {
    if (currentWordList.empty()) {
        return SpellingQuestion{};
    }
    
    Word currentWord = currentWordList.back();
    currentWordList.pop_back();
    
    return SpellingQuestion{
        currentWord,
        getQuestionText(currentWord),
        getHint(currentWord)
    };
}

bool Study::checkSpelling(const std::string& input, const std::string& correct) {
    if (input.length() != correct.length()) {
        return false;
    }
    
    // 不区分大小写比较
    for (size_t i = 0; i < input.length(); ++i) {
        if (std::tolower(input[i]) != std::tolower(correct[i])) {
            return false;
        }
    }
    
    return true;
}

bool Study::checkSpellingAnswer(const SpellingQuestion& question, const std::string& userInput) {
    std::string correctAnswer = getAnswerText(question.word);
    bool isCorrect;
    
    if (currentDirection == TranslationDirection::CHINESE_TO_ENGLISH) {
        // 中译英模式需要检查拼写
        isCorrect = checkSpelling(userInput, correctAnswer);
    } else {
        // 英译中模式直接比较
        isCorrect = (userInput == correctAnswer);
    }
    
    // 更新单词的学习统计
    if (isCorrect) {
        question.word.incrementCorrectCount();
    } else {
        vocabulary.addToWrongWords(question.word);
    }
    
    // 更新用户学习记录
    user.updateLearningRecord(question.word.getEnglish(), isCorrect);
    
    return isCorrect;
}

std::string Study::getHint(const Word& word) const {
    if (currentDirection == TranslationDirection::CHINESE_TO_ENGLISH) {
        // 中译英模式，提供英文单词的提示
        std::string english = word.getEnglish();
        std::string hint = english.substr(0, 2);  // 显示前两个字母
        for (size_t i = 2; i < english.length(); ++i) {
            hint += "_";
        }
        return hint;
    } else {
        // 英译中模式，提供词性提示
        return "词性: " + word.getPartOfSpeech();
    }
}

int Study::getRemainingWords() const {
    return currentWordList.size();
}

int Study::getTotalWords() const {
    return currentWordList.size();
}

double Study::getAccuracy() const {
    return user.getLearningAccuracy();
}

void Study::endSession() {
    user.saveToDatabase();
}

Study::StudyStatistics Study::getSessionStatistics() const {
    StudyStatistics stats;
    
    stats.totalAttempts = user.getTotalAttempts();
    stats.correctAnswers = user.getCorrectAnswers();
    stats.accuracy = user.getLearningAccuracy();
    stats.timeSpentMinutes = user.getLearningTime();
    stats.wrongWords = vocabulary.getWrongWords();
    stats.direction = currentDirection;
    
    return stats;
}
