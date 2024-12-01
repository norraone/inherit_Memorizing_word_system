#include "user_service.h"
#include <sstream>
#include <QDebug>

void UserService::login(const std::string& username, const std::string& password) {
    auto user = repository->findByUsername(username);
    if (!user) {
        throw AuthenticationError("用户名不存在");
    }
    
    if (!user->verifyPassword(password)) {
        throw AuthenticationError("密码错误");
    }
    
    currentUser = user;
}

void UserService::logout() {
    currentUser.reset();
}

void UserService::registerUser(const std::string& username, const std::string& password) {
    // Validate input
    if (username.empty() || password.empty()) {
        throw std::invalid_argument("用户名和密码不能为空");
    }
    
    // Check username length and format
    if (username.length() < 3 || username.length() > 20) {
        throw std::invalid_argument("用户名长度必须在3-20个字符之间");
    }
    
    // Check password complexity
    if (password.length() < 6) {
        throw std::invalid_argument("密码长度至少为6个字符");
    }
    
    // Check for existing username
    if (repository->findByUsername(username)) {
        throw std::runtime_error("用户名已存在");
    }
    
    // Create new user
    User newUser(username, password);
    
    // Attempt to save user
    if (!repository->save(newUser)) {
        // More specific error message
        throw std::runtime_error(
            "注册失败：无法保存用户信息。请检查数据库连接或权限。"
        );
    }
}

void UserService::updatePassword(const std::string& oldPassword, const std::string& newPassword) {
    if (!currentUser) {
        throw AuthenticationError("用户未登录");
    }
    
    if (!currentUser->verifyPassword(oldPassword)) {
        throw AuthenticationError("原密码错误");
    }
    
    if (newPassword.empty()) {
        throw std::invalid_argument("新密码不能为空");
    }
    
    User updatedUser = *currentUser;
    updatedUser.updatePassword(newPassword);
    
    if (!repository->update(updatedUser)) {
        throw std::runtime_error("更新密码失败");
    }
    
    *currentUser = updatedUser;
}

void UserService::checkIn() {
    // Ensure a user is logged in
    if (!currentUser) {
        throw std::runtime_error("未登录，无法签到");
    }
    
    // Perform check-in
    currentUser->checkIn();
    
    // Update user in the repository
    if (!repository->update(*currentUser)) {
        throw std::runtime_error("签到失败：无法更新用户信息");
    }
    
    // Prepare detailed check-in message
    std::stringstream message;
    message << "签到成功！\n"
            << "连续签到天数：" << currentUser->getStats().daysStreak << " 天\n"
            << "获得积分：" << currentUser->calculateCheckInPoints() << " 分";
    
    // Optional: Log the check-in event
    qDebug() << QString::fromStdString(message.str());
}

void UserService::addScore(int points) {
    if (!currentUser) {
        throw AuthenticationError("用户未登录");
    }
    
    User updatedUser = *currentUser;
    updatedUser.addScore(points);
    
    if (!repository->update(updatedUser)) {
        throw std::runtime_error("更新积分失败");
    }
    
    currentUser = updatedUser;
}

void UserService::recordWordLearned() {
    if (!currentUser) {
        throw AuthenticationError("用户未登录");
    }
    
    User updatedUser = *currentUser;
    updatedUser.recordWordLearned();
    
    if (!repository->update(updatedUser)) {
        throw std::runtime_error("更新学习记录失败");
    }
    
    currentUser = updatedUser;
}

std::vector<User> UserService::getLeaderboard(int limit) {
    return repository->getTopUsers(limit);
}

double UserService::getAverageWordsLearned() {
    return repository->getAverageWordsPerUser();
}

int UserService::getCurrentStreak() const {
    if (!currentUser) {
        throw AuthenticationError("用户未登录");
    }
    return currentUser->getStats().daysStreak;
}

int UserService::getTotalScore() const {
    if (!currentUser) {
        throw AuthenticationError("用户未登录");
    }
    return currentUser->getStats().totalScore;
}

// getCurrentUser() method is now fully defined in the header file
// No additional implementation needed
