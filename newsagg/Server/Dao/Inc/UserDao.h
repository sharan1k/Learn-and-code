#pragma once

#include "../Inc/DbConnection.h"
#include "../../Dto/Inc/User.h"
#include <memory>
#include <vector>
#include <string>

class UserDao {
public:
    UserDao();
    ~UserDao();
    bool createUser(const User& user);    
    std::shared_ptr<User> findById(unsigned int userId);
    std::shared_ptr<User> findByEmail(const std::string& email);
    std::shared_ptr<User> findByUserName(const std::string& userName);
    std::shared_ptr<User> verifyLogin(const std::string& email, const std::string& password);
    std::shared_ptr<User> verifyLoginByUsername(const std::string& userName, const std::string& password);
    bool updateUser(const User& user);
    bool deleteUser(unsigned int userId);
};
