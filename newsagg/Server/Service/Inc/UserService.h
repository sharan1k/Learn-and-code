#pragma once

#include "../../Dto/Inc/User.h"
#include "../Inc/UserService.h"
#include "../../Dao/Inc/UserDao.h"
#include <memory>
#include <string>

class UserService {
public:
    UserService();
    ~UserService();
    std::pair<bool, std::string> registerUser(const User& user, std::shared_ptr<User>& createdUser);
    std::shared_ptr<User> authenticateUser(const std::string& userName, const std::string& password);
    std::shared_ptr<User> authenticateUserByEmail(const std::string& email, const std::string& password);
    std::shared_ptr<User> getUserByEmail(const std::string& email);
    std::shared_ptr<User> getUserByUsername(const std::string& userName);
    std::shared_ptr<User> getUserById(unsigned int userId);
    bool updateUserProfile(const User& user);
    bool deleteUserAccount(unsigned int userId);

private:
    UserDao userDao;
};
