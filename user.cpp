#include "user.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <regex>

QSqlDatabase User::db = QSqlDatabase::addDatabase("QSQLITE");
bool User::dbInitialized = false;

void User::initializeDatabase() {
    if (!dbInitialized) {
        db.setDatabaseName("wordmem.db");
        if (db.open()) {
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
                      
            // 学习记录表
            query.exec("CREATE TABLE IF NOT EXISTS learning_records ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "username TEXT,"
                      "word TEXT,"
                      "correct INTEGER,"
                      "date TEXT,"
                      "FOREIGN KEY(username) REFERENCES users(username)"
                      ")");
            
            // 打卡记录表
            query.exec("CREATE TABLE IF NOT EXISTS checkin_records ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "username TEXT,"
                      "date TEXT,"
                      "streak_count INTEGER,"
                      "with_learning INTEGER,"
                      "FOREIGN KEY(username) REFERENCES users(username)"
                      ")");
            
            dbInitialized = true;
        }
    }
}

std::string User::hashPassword(const std::string& pwd) {
    QByteArray input = QByteArray::fromStdString(pwd);
    QByteArray hash = QCryptographicHash::hash(input, QCryptographicHash::Sha256);
    return hash.toHex().toStdString();
}

bool User::verifyPassword(const std::string& inputPwd) const {
    return hashPassword(inputPwd) == password;
}

bool User::usernameExists(const std::string& uname) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    query.addBindValue(QString::fromStdString(uname));
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool User::isValidUsername(const std::string& username) {
    // 用户名规则：4-20个字符，只能包含字母、数字和下划线
    std::regex pattern("^[a-zA-Z0-9_]{4,20}$");
    return std::regex_match(username, pattern);
}

bool User::isValidPassword(const std::string& password) {
    // 密码规则：8-20个字符，必须包含大小写字母和数字
    std::regex pattern("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{8,20}$");
    return std::regex_match(password, pattern);
}

User::RegistrationResult User::registerUser(
    const std::string& username,
    const std::string& password,
    const std::string& confirmPassword) {
    
    RegistrationResult result;
    
    // 检查用户名格式
    if (!isValidUsername(username)) {
        result.success = false;
        result.message = "用户名必须是4-20个字符，只能包含字母、数字和下划线";
        return result;
    }
    
    // 检查密码格式
    if (!isValidPassword(password)) {
        result.success = false;
        result.message = "密码必须是8-20个字符，包含大小写字母和数字";
        return result;
    }
    
    // 检查密码确认
    if (password != confirmPassword) {
        result.success = false;
        result.message = "两次输入的密码不一致";
        return result;
    }
    
    // 检查用户名是否已存在
    if (usernameExists(username)) {
        result.success = false;
        result.message = "用户名已存在";
        return result;
    }
    
    // 创建新用户
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (?, ?)");
    query.addBindValue(QString::fromStdString(username));
    query.addBindValue(QString::fromStdString(hashPassword(password)));
    
    if (query.exec()) {
        result.success = true;
        result.message = "注册成功";
    } else {
        result.success = false;
        result.message = "注册失败：数据库错误";
    }
    
    return result;
}

User::LoginResult User::login(const std::string& username, const std::string& password) {
    LoginResult result;
    
    // 检查用户是否存在
    if (!usernameExists(username)) {
        result.success = false;
        result.message = "用户名不存在";
        result.user = nullptr;
        return result;
    }
    
    // 创建用户对象并验证密码
    User* user = new User(username, password);
    if (!user->verifyPassword(password)) {
        delete user;
        result.success = false;
        result.message = "密码错误";
        result.user = nullptr;
        return result;
    }
    
    // 加载用户数据
    if (!user->loadUserData()) {
        delete user;
        result.success = false;
        result.message = "加载用户数据失败";
        result.user = nullptr;
        return result;
    }
    
    user->isLoggedIn = true;
    result.success = true;
    result.message = "登录成功";
    result.user = user;
    return result;
}

bool User::loadUserData() {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = ?");
    query.addBindValue(QString::fromStdString(username));
    
    if (query.exec() && query.next()) {
        totalScore = query.value("total_score").toInt();
        daysStreak = query.value("days_streak").toInt();
        totalWordsLearned = query.value("total_words_learned").toInt();
        lastCheckInDate = query.value("last_checkin_date").toString().toStdString();
        return true;
    }
    return false;
}

void User::logout() {
    // 保存用户数据
    saveToDatabase();
    isLoggedIn = false;
}

bool User::changePassword(const std::string& oldPassword, const std::string& newPassword) {
    // 验证旧密码
    if (!verifyPassword(oldPassword)) {
        return false;
    }
    
    // 检查新密码格式
    if (!isValidPassword(newPassword)) {
        return false;
    }
    
    // 更新密码
    password = hashPassword(newPassword);
    
    // 保存到数据库
    QSqlQuery query;
    query.prepare("UPDATE users SET password = ? WHERE username = ?");
    query.addBindValue(QString::fromStdString(password));
    query.addBindValue(QString::fromStdString(username));
    
    return query.exec();
}

bool User::saveToDatabase() {
    if (!isLoggedIn) return false;
    
    QSqlQuery query;
    query.prepare("UPDATE users SET "
                 "total_score = ?, "
                 "days_streak = ?, "
                 "total_words_learned = ?, "
                 "last_checkin_date = ? "
                 "WHERE username = ?");
                 
    query.addBindValue(totalScore);
    query.addBindValue(daysStreak);
    query.addBindValue(totalWordsLearned);
    query.addBindValue(QString::fromStdString(lastCheckInDate));
    query.addBindValue(QString::fromStdString(username));
    
    return query.exec();
}

void User::updateScore(int points) {
    totalScore += points;
    saveToDatabase();
}

void User::incrementWordsLearned() {
    ++totalWordsLearned;
    saveToDatabase();
}

bool User::addLearningRecord(const LearningRecord& record) {
    QSqlQuery query;
    query.prepare("INSERT INTO learning_records (username, word, correct, date) "
                 "VALUES (:username, :word, :correct, :date)");
    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":word", QString::fromStdString(record.word));
    query.bindValue(":correct", record.correct);
    query.bindValue(":date", QString::fromStdString(record.date));
    
    return query.exec();
}

std::vector<User::LearningRecord> User::getLearningHistory(int days) const {
    std::vector<LearningRecord> records;
    QSqlQuery query;
    
    QString queryStr = "SELECT word, correct, date FROM learning_records "
                      "WHERE username = :username";
    
    if (days > 0) {
        queryStr += " AND date >= date('now', '-" + QString::number(days) + " days')";
    }
    
    queryStr += " ORDER BY date DESC";
    
    query.prepare(queryStr);
    query.bindValue(":username", QString::fromStdString(username));
    
    if (query.exec()) {
        while (query.next()) {
            LearningRecord record;
            record.word = query.value(0).toString().toStdString();
            record.correct = query.value(1).toBool();
            record.date = query.value(2).toString().toStdString();
            records.push_back(record);
        }
    }
    
    return records;
}

bool User::addCheckInRecord(bool withLearning) {
    QSqlQuery query;
    query.prepare("INSERT INTO checkin_records (username, date, streak_count, with_learning) "
                 "VALUES (:username, date('now'), :streak_count, :with_learning)");
    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":streak_count", daysStreak);
    query.bindValue(":with_learning", withLearning);
    
    return query.exec();
}

std::vector<User::CheckInRecord> User::getCheckInHistory(int days) const {
    std::vector<CheckInRecord> records;
    QSqlQuery query;
    
    QString queryStr = "SELECT date, streak_count, with_learning FROM checkin_records "
                      "WHERE username = :username";
    
    if (days > 0) {
        queryStr += " AND date >= date('now', '-" + QString::number(days) + " days')";
    }
    
    queryStr += " ORDER BY date DESC";
    
    query.prepare(queryStr);
    query.bindValue(":username", QString::fromStdString(username));
    
    if (query.exec()) {
        while (query.next()) {
            CheckInRecord record;
            record.date = query.value(0).toString().toStdString();
            record.streakCount = query.value(1).toInt();
            record.withLearning = query.value(2).toBool();
            records.push_back(record);
        }
    }
    
    return records;
}

int User::getMonthlyCheckInCount() const {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM checkin_records "
                 "WHERE username = :username "
                 "AND date >= date('now', 'start of month')");
    query.bindValue(":username", QString::fromStdString(username));
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

User::~User() {
    if (isLoggedIn) {
        logout();
    }
}
