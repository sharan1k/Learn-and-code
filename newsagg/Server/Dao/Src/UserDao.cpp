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
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in createUser: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        std::cerr << "ErrorCode: " << e.getErrorCode() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Error in createUser: " << e.what() << std::endl;
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
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
       
        if (res->next()) {
            auto user = std::make_shared<User>();
            user->userId = res->getUInt("userId");
            user->userName = res->getString("userName");
            user->emailId = res->getString("emailId");
            user->role = res->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in findById: " << e.what() << std::endl;
        return nullptr;
    } catch (std::exception &e) {
        std::cerr << "Error in findById: " << e.what() << std::endl;
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
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            auto user = std::make_shared<User>();
            user->userId = res->getUInt("userId");
            user->userName = res->getString("userName");
            user->emailId = res->getString("emailId");
            user->role = res->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in findByEmail: " << e.what() << std::endl;
        return nullptr;
    } catch (std::exception &e) {
        std::cerr << "Error in findByEmail: " << e.what() << std::endl;
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
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            auto user = std::make_shared<User>();
            user->userId = res->getUInt("userId");
            user->userName = res->getString("userName");
            user->emailId = res->getString("emailId");
            user->role = res->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in verifyLogin: " << e.what() << std::endl;
        return nullptr;
    } catch (std::exception &e) {
        std::cerr << "Error in verifyLogin: " << e.what() << std::endl;
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
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in updateUser: " << e.what() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Error in updateUser: " << e.what() << std::endl;
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
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in deleteUser: " << e.what() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Error in deleteUser: " << e.what() << std::endl;
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
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            auto user = std::make_shared<User>();
            user->userId = res->getUInt("userId");
            user->userName = res->getString("userName");
            user->emailId = res->getString("emailId");
            user->role = res->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in findByUserName: " << e.what() << std::endl;
        return nullptr;
    } catch (std::exception &e) {
        std::cerr << "Error in findByUserName: " << e.what() << std::endl;
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
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            auto user = std::make_shared<User>();
            user->userId = res->getUInt("userId");
            user->userName = res->getString("userName");
            user->emailId = res->getString("emailId");
            user->role = res->getString("role");
            return user;
        }
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in verifyLoginByUsername: " << e.what() << std::endl;
        return nullptr;
    } catch (std::exception &e) {
        std::cerr << "Error in verifyLoginByUsername: " << e.what() << std::endl;
        return nullptr;
    }
}
