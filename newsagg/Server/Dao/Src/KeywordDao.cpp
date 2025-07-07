#include "../Inc/KeywordDao.h"
#include "../Inc/DbConnection.h"
#include <iostream>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

std::vector<std::string> KeywordDao::getByUser(unsigned int userId) {
    std::vector<std::string> keywords;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return keywords;
        }
        
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT keyword FROM keyword WHERE userId = " + std::to_string(userId)
        ));
        
        while (res->next()) {
            keywords.push_back(res->getString("keyword"));
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return keywords;
}

std::map<unsigned int, std::vector<std::string>> KeywordDao::getAll() {
    std::map<unsigned int, std::vector<std::string>> result;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return result;
        }
        
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT userId, keyword FROM keyword"
        ));
        
        while (res->next()) {
            unsigned int userId = res->getUInt("userId");
            std::string keyword = res->getString("keyword");
            
            result[userId].push_back(keyword);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return result;
}

bool KeywordDao::add(unsigned int userId, const std::string& keyword) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return false;
        }
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO keyword (userId, keyword) VALUES (?, ?)"
        ));
        
        pstmt->setUInt(1, userId);
        pstmt->setString(2, keyword);
        
        int affectedRows = pstmt->executeUpdate();
        return affectedRows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return false;
}

bool KeywordDao::remove(unsigned int userId, const std::string& keyword) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return false;
        }
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "DELETE FROM keyword WHERE userId = ? AND keyword = ?"
        ));
        
        pstmt->setUInt(1, userId);
        pstmt->setString(2, keyword);
        
        int affectedRows = pstmt->executeUpdate();
        return affectedRows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return false;
}
