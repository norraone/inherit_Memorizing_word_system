#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <string>
#include <stdexcept>

class Database {
private:
    static QSqlDatabase db;
    static bool initialized;
    
    // 私有构造函数防止实例化
    Database() {}
    
public:
    // 获取数据库连接
    static QSqlDatabase& getConnection() {
        if (!initialized) {
            initialize();
        }
        return db;
    }
    
    // 初始化数据库
    static void initialize() {
        if (!initialized) {
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("wordmem.db");
            
            if (!db.open()) {
                throw std::runtime_error("Failed to open database: " + 
                    db.lastError().text().toStdString());
            }
            
            createTables();
            initialized = true;
        }
    }
    
    // 关闭数据库连接
    static void close() {
        if (initialized) {
            db.close();
            initialized = false;
        }
    }
    
private:
    // 创建所有必要的表
    static void createTables() {
        QSqlQuery query;
        
        // 用户表
        query.exec("CREATE TABLE IF NOT EXISTS users ("
                  "username TEXT PRIMARY KEY,"
                  "password TEXT NOT NULL,"
                  "total_score INTEGER DEFAULT 0,"
                  "days_streak INTEGER DEFAULT 0,"
                  "total_words_learned INTEGER DEFAULT 0,"
                  "last_checkin_date TEXT,"
                  "created_at TEXT DEFAULT CURRENT_TIMESTAMP"
                  ")");
        
        // 单词表
        query.exec("CREATE TABLE IF NOT EXISTS words ("
                  "english TEXT PRIMARY KEY,"
                  "part_of_speech TEXT,"
                  "chinese TEXT,"
                  "frequency INTEGER DEFAULT 0,"
                  "correct_count INTEGER DEFAULT 0,"
                  "total_attempts INTEGER DEFAULT 0,"
                  "added_date TEXT DEFAULT CURRENT_TIMESTAMP"
                  ")");
        
        // 单词释义表
        query.exec("CREATE TABLE IF NOT EXISTS word_definitions ("
                  "english TEXT,"
                  "definition_type TEXT,"  // definition, example, synonym, antonym
                  "content TEXT,"
                  "added_date TEXT DEFAULT CURRENT_TIMESTAMP,"
                  "PRIMARY KEY (english, definition_type, content),"
                  "FOREIGN KEY(english) REFERENCES words(english)"
                  ")");
        
        // 单词分类表
        query.exec("CREATE TABLE IF NOT EXISTS word_categories ("
                  "english TEXT,"
                  "category TEXT,"
                  "added_date TEXT DEFAULT CURRENT_TIMESTAMP,"
                  "PRIMARY KEY (english, category),"
                  "FOREIGN KEY(english) REFERENCES words(english)"
                  ")");
        
        // 学习记录表
        query.exec("CREATE TABLE IF NOT EXISTS learning_records ("
                  "username TEXT,"
                  "word TEXT,"
                  "correct INTEGER,"
                  "timestamp TEXT DEFAULT CURRENT_TIMESTAMP,"
                  "FOREIGN KEY(username) REFERENCES users(username),"
                  "FOREIGN KEY(word) REFERENCES words(english)"
                  ")");
        
        // 错词本表
        query.exec("CREATE TABLE IF NOT EXISTS wrong_words ("
                  "username TEXT,"
                  "word TEXT,"
                  "added_date TEXT DEFAULT CURRENT_TIMESTAMP,"
                  "PRIMARY KEY (username, word),"
                  "FOREIGN KEY(username) REFERENCES users(username),"
                  "FOREIGN KEY(word) REFERENCES words(english)"
                  ")");
        
        // 每日统计表
        query.exec("CREATE TABLE IF NOT EXISTS daily_statistics ("
                  "username TEXT,"
                  "date TEXT,"
                  "score INTEGER DEFAULT 0,"
                  "words_learned INTEGER DEFAULT 0,"
                  "correct_count INTEGER DEFAULT 0,"
                  "wrong_count INTEGER DEFAULT 0,"
                  "study_time INTEGER DEFAULT 0,"  // 学习时间（分钟）
                  "PRIMARY KEY (username, date),"
                  "FOREIGN KEY(username) REFERENCES users(username)"
                  ")");
        
        // 打卡记录表
        query.exec("CREATE TABLE IF NOT EXISTS checkin_records ("
                  "username TEXT,"
                  "date TEXT,"
                  "with_learning BOOLEAN DEFAULT 0,"
                  "PRIMARY KEY (username, date),"
                  "FOREIGN KEY(username) REFERENCES users(username)"
                  ")");
    }
};

// 静态成员初始化
QSqlDatabase Database::db;
bool Database::initialized = false;

#endif // DATABASE_H
