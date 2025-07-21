#include "../Inc/UserDao.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>

UserDao::UserDao() {
}

UserDao::~UserDao() {
}

bool UserDao::createUser(const User& user) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO users (userName, emailId, password, role) VALUES (?, ?, ?, ?)"
        ));
        pstmt->setString(1, user.userName);
        pstmt->setString(2, user.emailId);
        pstmt->setString(3, user.password);
        pstmt->setString(4, user.role);
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in createUser: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        std::cerr << "ErrorCode: " << exception.getErrorCode() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Error in createUser: " << exception.what() << std::endl;
        return false;
    }
}

std::shared_ptr<User> UserDao::findById(unsigned int userId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM users WHERE userId = ?"
        ));
        pstmt->setUInt(1, userId);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
       
        if (response->next()) {
            auto user = std::make_shared<User>();
            user->userId = response->getUInt("userId");
            user->userName = response->getString("userName");
            user->emailId = response->getString("emailId");
            user->role = response->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in findById: " << exception.what() << std::endl;
        return nullptr;
    } catch (std::exception &exception) {
        std::cerr << "Error in findById: " << exception.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<User> UserDao::findByEmail(const std::string& email) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM users WHERE emailId = ?"
        ));
        pstmt->setString(1, email);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        if (response->next()) {
            auto user = std::make_shared<User>();
            user->userId = response->getUInt("userId");
            user->userName = response->getString("userName");
            user->emailId = response->getString("emailId");
            user->role = response->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in findByEmail: " << exception.what() << std::endl;
        return nullptr;
    } catch (std::exception &exception) {
        std::cerr << "Error in findByEmail: " << exception.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<User> UserDao::verifyLogin(const std::string& email, const std::string& password) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM users WHERE emailId = ? AND password = ?"
        ));
        pstmt->setString(1, email);
        pstmt->setString(2, password);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        if (response->next()) {
            auto user = std::make_shared<User>();
            user->userId = response->getUInt("userId");
            user->userName = response->getString("userName");
            user->emailId = response->getString("emailId");
            user->role = response->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in verifyLogin: " << exception.what() << std::endl;
        return nullptr;
    } catch (std::exception &exception) {
        std::cerr << "Error in verifyLogin: " << exception.what() << std::endl;
        return nullptr;
    }
}

bool UserDao::updateUser(const User& user) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "UPDATE users SET userName = ?, emailId = ?, role = ? WHERE userId = ?"
        ));
        pstmt->setString(1, user.userName);
        pstmt->setString(2, user.emailId);
        pstmt->setString(3, user.role);
        pstmt->setUInt(4, user.userId);
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0; 
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in updateUser: " << exception.what() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Error in updateUser: " << exception.what() << std::endl;
        return false;
    }
}

bool UserDao::deleteUser(unsigned int userId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "DELETE FROM users WHERE userId = ?"
        ));
        pstmt->setUInt(1, userId);
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;   
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in deleteUser: " << exception.what() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Error in deleteUser: " << exception.what() << std::endl;
        return false;
    }
}

std::shared_ptr<User> UserDao::findByUserName(const std::string& userName) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM users WHERE userName = ?"
        ));
        pstmt->setString(1, userName);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        if (response->next()) {
            auto user = std::make_shared<User>();
            user->userId = response->getUInt("userId");
            user->userName = response->getString("userName");
            user->emailId = response->getString("emailId");
            user->role = response->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in findByUserName: " << exception.what() << std::endl;
        return nullptr;
    } catch (std::exception &exception) {
        std::cerr << "Error in findByUserName: " << exception.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<User> UserDao::verifyLoginByUsername(const std::string& userName, const std::string& password) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM users WHERE userName = ? AND password = ?"
        ));
        pstmt->setString(1, userName);
        pstmt->setString(2, password);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        if (response->next()) {
            auto user = std::make_shared<User>();
            user->userId = response->getUInt("userId");
            user->userName = response->getString("userName");
            user->emailId = response->getString("emailId");
            user->role = response->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in verifyLoginByUsername: " << exception.what() << std::endl;
        return nullptr;
    } catch (std::exception &exception) {
        std::cerr << "Error in verifyLoginByUsername: " << exception.what() << std::endl;
        return nullptr;
    }
}
