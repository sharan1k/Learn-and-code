#pragma once

#include <gmock/gmock.h>
#include "../../Dao/Inc/UserDao.h"

class MockUserDao : public UserDao {
public:
    MockUserDao() : UserDao() {}
    ~MockUserDao() override = default;
    
    MOCK_METHOD(bool, createUser, (const User&));
    MOCK_METHOD(std::shared_ptr<User>, findByEmail, (const std::string&));
    MOCK_METHOD(std::shared_ptr<User>, findByUserName, (const std::string&));
    MOCK_METHOD(std::shared_ptr<User>, findById, (unsigned int));
    MOCK_METHOD(std::shared_ptr<User>, verifyLogin, (const std::string&, const std::string&));
    MOCK_METHOD(std::shared_ptr<User>, verifyLoginByUsername, (const std::string&, const std::string&));
    MOCK_METHOD(bool, updateUser, (const User&));
    MOCK_METHOD(bool, deleteUser, (unsigned int));
};
