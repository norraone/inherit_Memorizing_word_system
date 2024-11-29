#ifndef REVIEW_H
#define REVIEW_H

#include <vector>
#include <string>
#include <random>
#include "word.h"
#include "statistics.h"
#include "study.h"

class Review {
private:
    Statistics& stats;
    Study& study;
    std::vector<Word> reviewWords;
    std::random_device rd;
    std::mt19937 gen;
    
    // 鼓励信息
    const std::vector<std::string> encouragements = {
        "太棒了！你已经完全掌握了这些单词！",
        "出色的表现！继续保持！",
        "完美！你的努力得到了回报！",
        "了不起！你的词汇量在稳步提升！",
        "厉害！这些单词对你来说已经不在话下了！"
    };
    
    // 获取随机鼓励信息
    std::string getRandomEncouragement() const;
    
    // 检查是否全部答对
    bool isAllCorrect() const;

public:
    Review(Statistics& s, Study& st);
    
    // 开始错题复习
    void startWrongWordsReview(int wordCount = 10);
    
    // 开始普通复习
    void startNormalReview(const std::vector<Word>& words);
    
    // 获取复习进度
    struct ReviewProgress {
        int totalWords;
        int completedWords;
        int correctCount;
        double accuracy;
        bool isFinished;
    };
    
    ReviewProgress getProgress() const;
    
    // 获取下一个复习题目
    Study::MultipleChoiceQuestion getNextMultipleChoice();
    Study::SpellingQuestion getNextSpelling();
    
    // 检查答案并更新统计
    bool checkAnswer(const Study::MultipleChoiceQuestion& question, int selectedIndex);
    bool checkAnswer(const Study::SpellingQuestion& question, const std::string& answer);
    
    // 获取复习结果
    struct ReviewResult {
        int totalWords;
        int correctCount;
        double accuracy;
        std::vector<Word> remainingWrongWords;
        std::string encouragementMessage;
    };
    
    ReviewResult getResult() const;
    
    // 重置复习状态
    void reset();
    
    // 获取剩余单词数
    int getRemainingWords() const;
    
    // 是否复习完成
    bool isFinished() const;
};

#endif // REVIEW_H
