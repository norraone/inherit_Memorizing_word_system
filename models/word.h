#ifndef WORD_H
#define WORD_H

#include <string>
#include <vector>
#include <chrono>
#include <optional>

class Word {
public:
    struct Definition {
        std::string type;
        std::string content;
        std::chrono::system_clock::time_point addedDate;
    };

    struct LearningStats {
        int frequency = 0;
        int correctCount = 0;
        int totalAttempts = 0;
        double getAccuracy() const { 
            return totalAttempts > 0 ? static_cast<double>(correctCount) / totalAttempts : 0.0; 
        }
    };

private:
    std::string english;
    std::string partOfSpeech;
    std::string chinese;
    std::vector<std::string> categories;
    std::vector<Definition> definitions;
    LearningStats stats;
    std::chrono::system_clock::time_point addedDate;

public:
    Word() = default;
    Word(std::string eng, std::string pos, std::string chn);

    // Getters
    const std::string& getEnglish() const { return english; }
    const std::string& getPartOfSpeech() const { return partOfSpeech; }
    const std::string& getChinese() const { return chinese; }
    const std::vector<std::string>& getCategories() const { return categories; }
    const std::vector<Definition>& getDefinitions() const { return definitions; }
    const LearningStats& getStats() const { return stats; }

    // Setters
    void setEnglish(const std::string& eng) { english = eng; }
    void setPartOfSpeech(const std::string& pos) { partOfSpeech = pos; }
    void setChinese(const std::string& chn) { chinese = chn; }
    
    // Learning methods
    void recordAttempt(bool correct);
    void addDefinition(const std::string& type, const std::string& content);
    void addCategory(const std::string& category);
    void removeCategory(const std::string& category);
};

#endif // WORD_H
