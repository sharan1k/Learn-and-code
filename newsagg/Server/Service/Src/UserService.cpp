#include "../Inc/UserService.h"
#include <iostream>

UserService::UserService() {}

UserService::~UserService() {}

std::pair<bool, std::string> UserService::registerUser(const User& user, std::shared_ptr<User>& createdUser) {
    if (user.userName.empty() || user.emailId.empty() || user.password.empty()) {
        return {false, "Invalid user data. Username, email, and password are required."};
    }
    
    auto existingUser = userDao.findByEmail(user.emailId);
    if (existingUser != nullptr) {
        return {false, "Email already registered."};
    }

    auto existingUsername = userDao.findByUserName(user.userName);
    if (existingUsername != nullptr) {
        return {false, "Username already taken."};
    }
    
    if (userDao.createUser(user)) {
        createdUser = userDao.findByEmail(user.emailId);
        if (createdUser != nullptr) {
            return {true, "User registered successfully."};
        }
    }
    
    return {false, "Failed to register user."};
}

std::shared_ptr<User> UserService::authenticateUser(const std::string& userName, const std::string& password) {
    if (userName.empty() || password.empty()) {
        return nullptr;
    }
    
    return userDao.verifyLoginByUsername(userName, password);
}

std::shared_ptr<User> UserService::authenticateUserByEmail(const std::string& email, const std::string& password) {
    if (email.empty() || password.empty()) {
        return nullptr;
    }
    
    return userDao.verifyLogin(email, password);
}

std::shared_ptr<User> UserService::getUserByEmail(const std::string& email) {
    return userDao.findByEmail(email);
}

std::shared_ptr<User> UserService::getUserByUsername(const std::string& userName) {
    return userDao.findByUserName(userName);
}

std::shared_ptr<User> UserService::getUserById(unsigned int userId) {
    return userDao.findById(userId);
}

bool UserService::updateUserProfile(const User& user) {
    return userDao.updateUser(user);
}

bool UserService::deleteUserAccount(unsigned int userId) {
    return userDao.deleteUser(userId);
}
