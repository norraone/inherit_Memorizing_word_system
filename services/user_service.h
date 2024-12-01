#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "../repositories/user_repository.h"
#include <memory>
#include <optional>
#include <stdexcept>

class AuthenticationError : public std::runtime_error {
public:
    explicit AuthenticationError(const std::string& message)
        : std::runtime_error(message) {}
};

class UserService {
private:
    std::unique_ptr<UserRepository> repository;
    std::optional<User> currentUser;

public:
    UserService() : repository(std::make_unique<UserRepository>()) {}
    
    // Authentication
    void login(const std::string& username, const std::string& password);
    void logout();
    void registerUser(const std::string& username, const std::string& password);
    bool isLoggedIn() const { return currentUser.has_value(); }
    const User& getCurrentUser() const;
    
    // User operations
    void updatePassword(const std::string& oldPassword, const std::string& newPassword);
    void checkIn();
    void addScore(int points);
    void recordWordLearned();
    
    // Statistics
    std::vector<User> getLeaderboard(int limit = 10);
    double getAverageWordsLearned();
    int getCurrentStreak() const;
    int getTotalScore() const;
};

#endif // USER_SERVICE_H
