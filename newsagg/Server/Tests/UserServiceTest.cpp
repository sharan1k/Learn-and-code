#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../Service/Inc/UserService.h"
#include "Mocks/MockUserDao.h"

using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;

class TestableUserService {
private:
    UserDao& userDao;

public:
    explicit TestableUserService(UserDao& dao) : userDao(dao) {}

    std::pair<bool, std::string> registerUser(const User& user, std::shared_ptr<User>& createdUser) {
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

    std::shared_ptr<User> authenticateUser(const std::string& userName, const std::string& password) {
        if (userName.empty() || password.empty()) {
            return nullptr;
        }
        
        return userDao.verifyLoginByUsername(userName, password);
    }

    std::shared_ptr<User> authenticateUserByEmail(const std::string& email, const std::string& password) {
        if (email.empty() || password.empty()) {
            return nullptr;
        }
        
        return userDao.verifyLogin(email, password);
    }

    std::shared_ptr<User> getUserByEmail(const std::string& email) {
        return userDao.findByEmail(email);
    }

    std::shared_ptr<User> getUserByUsername(const std::string& userName) {
        return userDao.findByUserName(userName);
    }

    std::shared_ptr<User> getUserById(unsigned int userId) {
        return userDao.findById(userId);
    }

    bool updateUserProfile(const User& user) {
        return userDao.updateUser(user);
    }

    bool deleteUserAccount(unsigned int userId) {
        return userDao.deleteUser(userId);
    }
};

class UserServiceTest : public ::testing::Test {
protected:
    NiceMock<MockUserDao> mockUserDao;
    std::unique_ptr<TestableUserService> service;
    
    void SetUp() override {
        service = std::make_unique<TestableUserService>(mockUserDao);
    }
};

TEST_F(UserServiceTest, RegisterUser_Success) {
    User testUser;
    testUser.userName = "testuser";
    testUser.emailId = "test@example.com";
    testUser.password = "password123";

    EXPECT_CALL(mockUserDao, findByEmail(testUser.emailId))
        .WillOnce(Return(nullptr));  
    
    EXPECT_CALL(mockUserDao, findByUserName(testUser.userName))
        .WillOnce(Return(nullptr));  
    
    EXPECT_CALL(mockUserDao, createUser(testing::Ref(testUser)))
        .WillOnce(Return(true));
    
    auto createdUser = std::make_shared<User>();
    createdUser->userName = testUser.userName;
    createdUser->emailId = testUser.emailId;
    createdUser->userId = 1;
    
    EXPECT_CALL(mockUserDao, findByEmail(testUser.emailId))
        .WillOnce(Return(createdUser));
    
    TestableUserService service(&mockUserDao);
    
    std::shared_ptr<User> resultUser;
    auto result = service.registerUser(testUser, resultUser);
    
    EXPECT_TRUE(result.first);
    EXPECT_EQ("User registered successfully.", result.second);
    ASSERT_NE(nullptr, resultUser);
    EXPECT_EQ(1U, resultUser->userId);
    EXPECT_EQ("testuser", resultUser->userName);
    EXPECT_EQ("test@example.com", resultUser->emailId);
}

TEST_F(UserServiceTest, RegisterUser_EmailAlreadyExists) {
    User testUser;
    testUser.userName = "testuser";
    testUser.emailId = "test@example.com";
    testUser.password = "password123";
    
    auto existingUser = std::make_shared<User>();
    existingUser->emailId = testUser.emailId;
    
    EXPECT_CALL(mockUserDao, findByEmail(testUser.emailId))
        .WillOnce(Return(existingUser)); 
    
    TestableUserService service(&mockUserDao);
    
    std::shared_ptr<User> resultUser;
    auto result = service.registerUser(testUser, resultUser);
    
    EXPECT_FALSE(result.first);
    EXPECT_EQ("Email already registered.", result.second);
}

TEST_F(UserServiceTest, RegisterUser_UsernameAlreadyExists) {
    User testUser;
    testUser.userName = "testuser";
    testUser.emailId = "test@example.com";
    testUser.password = "password123";
    
    EXPECT_CALL(mockUserDao, findByEmail(testUser.emailId))
        .WillOnce(Return(nullptr));
    
    auto existingUser = std::make_shared<User>();
    existingUser->userName = testUser.userName;
    
    EXPECT_CALL(mockUserDao, findByUserName(testUser.userName))
        .WillOnce(Return(existingUser));
    
    TestableUserService service(&mockUserDao);

    std::shared_ptr<User> resultUser;
    auto result = service.registerUser(testUser, resultUser);
    
    EXPECT_FALSE(result.first);
    EXPECT_EQ("Username already taken.", result.second);
}

TEST_F(UserServiceTest, RegisterUser_MissingRequiredFields) {
    User testUser; 
    TestableUserService service(&mockUserDao);
    std::shared_ptr<User> resultUser;
    auto result = service.registerUser(testUser, resultUser);
    
    EXPECT_FALSE(result.first);
    EXPECT_EQ("Invalid user data. Username, email, and password are required.", result.second);
}

TEST_F(UserServiceTest, AuthenticateUser_Success) {
    std::string username = "testuser";
    std::string password = "password123";
    
    auto user = std::make_shared<User>();
    user->userName = username;
    user->userId = 1;
    
    EXPECT_CALL(mockUserDao, verifyLoginByUsername(username, password))
        .WillOnce(Return(user));
    
    TestableUserService service(&mockUserDao);
    
    auto result = service.authenticateUser(username, password);
    
    ASSERT_NE(nullptr, result);
    EXPECT_EQ(username, result->userName);
    EXPECT_EQ(1U, result->userId);
}

TEST_F(UserServiceTest, AuthenticateUser_InvalidCredentials) {
    std::string username = "testuser";
    std::string password = "wrongpassword";
    
    EXPECT_CALL(mockUserDao, verifyLoginByUsername(username, password))
        .WillOnce(Return(nullptr));
    
    TestableUserService service(&mockUserDao);
    
    auto result = service.authenticateUser(username, password);
    
    EXPECT_EQ(nullptr, result);
}

TEST_F(UserServiceTest, AuthenticateUserByEmail_Success) {
    std::string email = "test@example.com";
    std::string password = "password123";
    
    auto user = std::make_shared<User>();
    user->emailId = email;
    user->userId = 1;
    
    EXPECT_CALL(mockUserDao, verifyLogin(email, password))
        .WillOnce(Return(user));
    
    TestableUserService service(&mockUserDao);
    
    auto result = service.authenticateUserByEmail(email, password);
    
    ASSERT_NE(nullptr, result);
    EXPECT_EQ(email, result->emailId);
    EXPECT_EQ(1U, result->userId);
}

TEST_F(UserServiceTest, GetUserByEmail_Success) {
    std::string email = "test@example.com";
    
    auto user = std::make_shared<User>();
    user->emailId = email;
    user->userId = 1;
    
    EXPECT_CALL(mockUserDao, findByEmail(email))
        .WillOnce(Return(user));
    
    TestableUserService service(&mockUserDao);
    
    auto result = service.getUserByEmail(email);
    
    ASSERT_NE(nullptr, result);
    EXPECT_EQ(email, result->emailId);
    EXPECT_EQ(1U, result->userId);
}

TEST_F(UserServiceTest, GetUserByUsername_Success) {
    std::string username = "testuser";
    
    auto user = std::make_shared<User>();
    user->userName = username;
    user->userId = 1;
    
    EXPECT_CALL(mockUserDao, findByUserName(username))
        .WillOnce(Return(user));
    
    TestableUserService service(&mockUserDao);
    
    auto result = service.getUserByUsername(username);
    
    ASSERT_NE(nullptr, result);
    EXPECT_EQ(username, result->userName);
    EXPECT_EQ(1U, result->userId);
}

TEST_F(UserServiceTest, GetUserById_Success) {
    unsigned int userId = 1;
    
    auto user = std::make_shared<User>();
    user->userId = userId;
    
    EXPECT_CALL(mockUserDao, findById(userId))
        .WillOnce(Return(user));
    
    TestableUserService service(&mockUserDao);
    
    auto result = service.getUserById(userId);
    
    ASSERT_NE(nullptr, result);
    EXPECT_EQ(userId, result->userId);
}

TEST_F(UserServiceTest, UpdateUserProfile_Success) {
    User user;
    user.userId = 1;
    user.userName = "updateduser";
    
    EXPECT_CALL(mockUserDao, updateUser(testing::Ref(user)))
        .WillOnce(Return(true));
    
    TestableUserService service(&mockUserDao);
    
    bool result = service.updateUserProfile(user);
    
    EXPECT_TRUE(result);
}

TEST_F(UserServiceTest, DeleteUserAccount_Success) {
    unsigned int userId = 1;
    
    EXPECT_CALL(mockUserDao, deleteUser(userId))
        .WillOnce(Return(true));
    
    TestableUserService service(&mockUserDao);
    
    bool result = service.deleteUserAccount(userId);
    
    EXPECT_TRUE(result);
}
