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
                      "username TEXT,"
                      "word TEXT,"
                      "correct INTEGER,"
                      "timestamp TEXT DEFAULT CURRENT_TIMESTAMP,"
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

User::User() : totalScore(0), daysStreak(0), totalWordsLearned(0), lastCheckInDate(""), isLoggedIn(false) {
    initializeDatabase();
}

User::User(const std::string& uname, const std::string& pwd)
    : username(uname), password(hashPassword(pwd)),
      totalScore(0), daysStreak(0), totalWordsLearned(0), lastCheckInDate(""), isLoggedIn(false) {
    initializeDatabase();
}

User::~User() {
    // 析构函数不需要关闭数据库，因为它是静态的
}

// 静态成员初始化
QSqlDatabase User::db;
bool User::dbInitialized = false;

// Getters
std::string User::getUsername() const { return username; }
int User::getTotalScore() const { return totalScore; }
int User::getDaysStreak() const { return daysStreak; }
int User::getTotalWordsLearned() const { return totalWordsLearned; }
std::string User::getLastCheckInDate() const { return lastCheckInDate; }

// 学习进度更新
void User::updateScore(int points) {
    totalScore += points;
    saveToDatabase();
}

void User::incrementDaysStreak() {
    daysStreak++;
    saveToDatabase();
}

void User::resetDaysStreak() {
    daysStreak = 0;
    saveToDatabase();
}

void User::incrementWordsLearned() {
    totalWordsLearned++;
    saveToDatabase();
}

// 学习记录
bool User::addLearningRecord(const LearningRecord& record) {
    QSqlQuery query;
    query.prepare("INSERT INTO learning_records (username, word, correct) VALUES (?, ?, ?)");
    
    query.addBindValue(QString::fromStdString(username));
    query.addBindValue(QString::fromStdString(record.word));
    query.addBindValue(record.correct);
    
    return query.exec();
}

std::vector<User::LearningRecord> User::getLearningHistory(int days) const {
    std::vector<LearningRecord> history;
    QSqlQuery query;
    
    QString queryStr = "SELECT word, correct FROM learning_records "
                      "WHERE username = ? AND timestamp >= date('now', '-%1 days') "
                      "ORDER BY timestamp DESC";
    
    query.prepare(queryStr.arg(days));
    query.addBindValue(QString::fromStdString(username));
    
    if (query.exec()) {
        while (query.next()) {
            LearningRecord record;
            record.word = query.value("word").toString().toStdString();
            record.correct = query.value("correct").toInt();
            history.push_back(record);
        }
    }
    
    return history;
}

// 打卡相关方法
bool User::checkIn() {
    if (hasCheckedInToday()) {
        return false;  // 今天已经打卡
    }

    QDate currentDate = QDate::currentDate();
    QDate lastDate = QDate::fromString(QString::fromStdString(lastCheckInDate), "yyyy-MM-dd");
    
    // 检查是否需要重置连续打卡
    if (needResetStreak()) {
        resetDaysStreak();
    }
    
    // 更新打卡信息
    incrementDaysStreak();
    lastCheckInDate = currentDate.toString("yyyy-MM-dd").toStdString();
    
    // 记录打卡
    bool withLearning = (totalWordsLearned > 0);  // 今天是否学习了单词
    return addCheckInRecord(withLearning);
}

bool User::hasCheckedInToday() const {
    if (lastCheckInDate.empty()) {
        return false;
    }
    
    QDate currentDate = QDate::currentDate();
    QDate lastDate = QDate::fromString(QString::fromStdString(lastCheckInDate), "yyyy-MM-dd");
    return currentDate == lastDate;
}

int User::getMissedDays() const {
    if (lastCheckInDate.empty()) {
        return 0;
    }
    
    QDate currentDate = QDate::currentDate();
    QDate lastDate = QDate::fromString(QString::fromStdString(lastCheckInDate), "yyyy-MM-dd");
    return lastDate.daysTo(currentDate) - 1;  // -1 因为今天还可以打卡
}

bool User::needResetStreak() const {
    if (lastCheckInDate.empty()) {
        return true;
    }
    
    QDate currentDate = QDate::currentDate();
    QDate lastDate = QDate::fromString(QString::fromStdString(lastCheckInDate), "yyyy-MM-dd");
    return lastDate.daysTo(currentDate) > 1;  // 超过1天没打卡就需要重置
}

bool User::addCheckInRecord(bool withLearning) {
    QSqlQuery query;
    query.prepare("INSERT INTO checkin_records (username, date, streak_count, with_learning) "
                 "VALUES (?, ?, ?, ?)");
    
    query.addBindValue(QString::fromStdString(username));
    query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    query.addBindValue(daysStreak);
    query.addBindValue(withLearning);
    
    return query.exec();
}

std::vector<User::CheckInRecord> User::getCheckInHistory(int days) const {
    std::vector<CheckInRecord> history;
    QSqlQuery query;
    
    QString queryStr = "SELECT date, streak_count, with_learning FROM checkin_records "
                      "WHERE username = ? AND date >= date('now', '-%1 days') "
                      "ORDER BY date DESC";
    
    query.prepare(queryStr.arg(days));
    query.addBindValue(QString::fromStdString(username));
    
    if (query.exec()) {
        while (query.next()) {
            CheckInRecord record;
            record.date = query.value("date").toString().toStdString();
            record.streakCount = query.value("streak_count").toInt();
            record.withLearning = query.value("with_learning").toBool();
            history.push_back(record);
        }
    }
    
    return history;
}

int User::getMonthlyCheckInCount() const {
    QSqlQuery query;
    QString queryStr = "SELECT COUNT(*) FROM checkin_records "
                      "WHERE username = ? AND "
                      "date >= date('now', 'start of month') AND "
                      "date <= date('now', 'end of month')";
    
    query.prepare(queryStr);
    query.addBindValue(QString::fromStdString(username));
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
