#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "base_repository.h"
#include "../models/user.h"
#include <optional>
#include <vector>

class UserRepository : public BaseRepository {
public:
    std::optional<User> findByUsername(const std::string& username);
    bool save(const User& user);
    bool update(const User& user);
    bool remove(const std::string& username);
    
    // Statistics and rankings
    std::vector<User> getTopUsers(int limit = 10);
    double getAverageWordsPerUser();
    std::vector<User> getUsersByStreak(int minStreak);
};

#endif // USER_REPOSITORY_H
