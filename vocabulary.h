#ifndef VOCABULARY_H
#define VOCABULARY_H

#include <vector>
#include <string>
#include "word.h"

class Vocabulary {
private:
    void initializeDatabase();

public:
    Vocabulary();
    
    // 基本词库维护
    bool addWord(const Word& word);
    bool removeWord(const std::string& english);
    bool modifyWord(const std::string& english, const Word& newWord);
    Word* findWord(const std::string& english);
    
    // 补充释义
    bool addDefinition(const std::string& english, const std::string& newDefinition);
    bool addExample(const std::string& english, const std::string& example);
    bool addSynonym(const std::string& english, const std::string& synonym);
    bool addAntonym(const std::string& english, const std::string& antonym);
    
    // 词库查询
    std::vector<Word> searchWords(const std::string& keyword);  // 搜索单词
    std::vector<Word> getWordsByPartOfSpeech(const std::string& pos);  // 按词性查找
    std::vector<Word> getRandomWords(int count);  // 随机获取单词
    
    // 词库统计
    int getTotalWords() const;  // 总单词数
    int getWrongWordsCount() const;  // 错词数量
    std::vector<std::string> getAllPartsOfSpeech() const;  // 获取所有词性
    
    // 错词本管理
    void addToWrongWords(const Word& word);
    void removeFromWrongWords(const Word& word);
    std::vector<Word> getWrongWords();
    
    // 高级查询
    std::vector<Word> getMostFrequentWords(int limit);  // 最常用单词
    std::vector<Word> getMostDifficultWords(int limit);  // 最难掌握的单词
    std::vector<Word> getRecentlyAddedWords(int days);  // 最近添加的单词
    
    // 词库导入导出
    bool importFromFile(const std::string& filename);  // 从文件导入单词
    bool exportToFile(const std::string& filename) const;  // 导出到文件
    
    // 词库分类
    bool addWordToCategory(const std::string& english, const std::string& category);
    bool removeWordFromCategory(const std::string& english, const std::string& category);
    std::vector<Word> getWordsByCategory(const std::string& category);
    std::vector<std::string> getAllCategories() const;
};

#endif // VOCABULARY_H
