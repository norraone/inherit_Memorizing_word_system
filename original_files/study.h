#ifndef STUDY_H
#define STUDY_H

#include <vector>
#include <string>
#include <random>
#include "word.h"
#include "vocabulary.h"
#include "user.h"

// 学习模式：选择题或拼写
enum class StudyMode {
    MULTIPLE_CHOICE,    // 选择题模式
    SPELLING           // 拼写模式
};

// 翻译方向：中译英或英译中
enum class TranslationDirection {
    CHINESE_TO_ENGLISH,    // 中译英
    ENGLISH_TO_CHINESE     // 英译中
};

class Study {
private:
    User& user;
    Vocabulary& vocabulary;
    std::vector<Word> currentWordList;
    std::random_device rd;
    std::mt19937 gen;
    TranslationDirection currentDirection;

    // 生成选择题的干扰选项
    std::vector<std::string> generateOptions(const Word& correctWord, TranslationDirection direction);
    
    // 随机打乱选项顺序
    void shuffleOptions(std::vector<std::string>& options);
    
    // 检查拼写是否正确
    bool checkSpelling(const std::string& input, const std::string& correct);

    // 获取问题文本（根据翻译方向）
    std::string getQuestionText(const Word& word) const;
    
    // 获取答案文本（根据翻译方向）
    std::string getAnswerText(const Word& word) const;

public:
    Study(User& u, Vocabulary& v);
    
    // 开始新的学习会话
    void startNewSession(int wordCount = 10, TranslationDirection direction = TranslationDirection::ENGLISH_TO_CHINESE);
    
    // 设置翻译方向
    void setTranslationDirection(TranslationDirection direction);
    TranslationDirection getTranslationDirection() const;
    
    // 选择题模式
    struct MultipleChoiceQuestion {
        Word word;                           // 当前单词
        std::string questionText;            // 问题文本（根据翻译方向可能是中文或英文）
        std::vector<std::string> options;    // 选项列表
        int correctIndex;                    // 正确答案的索引
    };
    
    MultipleChoiceQuestion getNextMultipleChoice();
    bool checkMultipleChoiceAnswer(const MultipleChoiceQuestion& question, int selectedIndex);
    
    // 拼写/输入模式
    struct SpellingQuestion {
        Word word;
        std::string questionText;    // 问题文本
        std::string hint;           // 提示信息
    };
    
    SpellingQuestion getNextSpellingQuestion();
    bool checkSpellingAnswer(const SpellingQuestion& question, const std::string& userInput);
    
    // 获取学习进度
    int getRemainingWords() const;
    int getTotalWords() const;
    double getAccuracy() const;
    
    // 获取提示
    std::string getHint(const Word& word) const;
    
    // 结束学习会话并保存进度
    void endSession();
    
    // 获取学习统计
    struct StudyStatistics {
        int totalAttempts;       // 总尝试次数
        int correctAnswers;      // 正确答案数
        double accuracy;         // 正确率
        int timeSpentMinutes;   // 学习时间（分钟）
        std::vector<Word> wrongWords;  // 错误的单词
        TranslationDirection direction; // 学习方向
    };
    
    StudyStatistics getSessionStatistics() const;
};

#endif // STUDY_H
