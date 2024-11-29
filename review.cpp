#include "review.h"
#include <algorithm>
#include <ctime>

Review::Review(Statistics& s, Study& st)
    : stats(s), study(st), gen(rd()) {
}

void Review::startWrongWordsReview(int wordCount) {
    // 获取错题本中的单词
    std::vector<Word> wrongWords = stats.getWrongWords();
    
    // 如果错题数量不足，就全部使用
    if (wordCount > wrongWords.size()) {
        reviewWords = wrongWords;
    } else {
        // 随机选择指定数量的错题
        std::shuffle(wrongWords.begin(), wrongWords.end(), gen);
        reviewWords.assign(wrongWords.begin(), wrongWords.begin() + wordCount);
    }
}

void Review::startNormalReview(const std::vector<Word>& words) {
    reviewWords = words;
    std::shuffle(reviewWords.begin(), reviewWords.end(), gen);
}

std::string Review::getRandomEncouragement() const {
    if (encouragements.empty()) {
        return "做得好！";
    }
    
    std::uniform_int_distribution<> dis(0, encouragements.size() - 1);
    return encouragements[dis(gen)];
}

bool Review::isAllCorrect() const {
    auto progress = getProgress();
    return progress.isFinished && progress.correctCount == progress.totalWords;
}

Review::ReviewProgress Review::getProgress() const {
    ReviewProgress progress;
    progress.totalWords = reviewWords.size();
    progress.completedWords = progress.totalWords - getRemainingWords();
    
    // 从 Study 类获取正确答题数
    auto studyStats = study.getSessionStatistics();
    progress.correctCount = studyStats.correctAnswers;
    
    // 计算正确率
    progress.accuracy = progress.completedWords > 0 
        ? static_cast<double>(progress.correctCount) / progress.completedWords 
        : 0.0;
        
    progress.isFinished = isFinished();
    
    return progress;
}

Study::MultipleChoiceQuestion Review::getNextMultipleChoice() {
    if (reviewWords.empty()) {
        return Study::MultipleChoiceQuestion{};
    }
    
    // 使用 Study 类生成选择题
    study.startNewSession(1);  // 只复习一个单词
    return study.getNextMultipleChoice();
}

Study::SpellingQuestion Review::getNextSpelling() {
    if (reviewWords.empty()) {
        return Study::SpellingQuestion{};
    }
    
    // 使用 Study 类生成拼写题
    study.startNewSession(1);  // 只复习一个单词
    return study.getNextSpellingQuestion();
}

bool Review::checkAnswer(const Study::MultipleChoiceQuestion& question, int selectedIndex) {
    bool isCorrect = study.checkMultipleChoiceAnswer(question, selectedIndex);
    
    if (isCorrect) {
        // 如果答对了，从错题本中移除
        stats.removeFromWrongWords(question.word);
    }
    
    // 更新统计信息
    stats.updateScore(isCorrect);
    
    // 如果全部答对，显示鼓励信息
    if (isAllCorrect()) {
        // 在这里可以触发UI显示鼓励信息
        getRandomEncouragement();
    }
    
    return isCorrect;
}

bool Review::checkAnswer(const Study::SpellingQuestion& question, const std::string& answer) {
    bool isCorrect = study.checkSpellingAnswer(question, answer);
    
    if (isCorrect) {
        // 如果答对了，从错题本中移除
        stats.removeFromWrongWords(question.word);
    }
    
    // 更新统计信息
    stats.updateScore(isCorrect);
    
    // 如果全部答对，显示鼓励信息
    if (isAllCorrect()) {
        // 在这里可以触发UI显示鼓励信息
        getRandomEncouragement();
    }
    
    return isCorrect;
}

Review::ReviewResult Review::getResult() const {
    ReviewResult result;
    auto progress = getProgress();
    
    result.totalWords = progress.totalWords;
    result.correctCount = progress.correctCount;
    result.accuracy = progress.accuracy;
    
    // 获取剩余的错词
    result.remainingWrongWords = stats.getWrongWords();
    
    // 如果全部答对，添加鼓励信息
    if (isAllCorrect()) {
        result.encouragementMessage = getRandomEncouragement();
    }
    
    return result;
}

void Review::reset() {
    reviewWords.clear();
}

int Review::getRemainingWords() const {
    return reviewWords.size();
}

bool Review::isFinished() const {
    return reviewWords.empty();
}
