#include "user_service.h"

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
    if (username.empty() || password.empty()) {
        throw std::invalid_argument("用户名和密码不能为空");
    }
    
    if (repository->findByUsername(username)) {
        throw std::runtime_error("用户名已存在");
    }
    
    User newUser(username, password);
    if (!repository->save(newUser)) {
        throw std::runtime_error("注册失败");
    }
}

const User& UserService::getCurrentUser() const {
    if (!currentUser) {
        throw AuthenticationError("用户未登录");
    }
    return *currentUser;
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
    if (!currentUser) {
        throw AuthenticationError("用户未登录");
    }
    
    if (currentUser->getStats().hasCheckedInToday()) {
        throw std::runtime_error("今天已经签到过了");
    }
    
    User updatedUser = *currentUser;
    updatedUser.checkIn();
    
    if (!repository->update(updatedUser)) {
        throw std::runtime_error("签到失败");
    }
    
    currentUser = updatedUser;
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
