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
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery(
            "SELECT keyword FROM keyword WHERE userId = " + std::to_string(userId)
        ));
        
        while (response->next()) {
            keywords.push_back(response->getString("keyword"));
        }
    } catch (sql::SQLException& exception) {
        std::cerr << "SQL Exception: " << exception.what() << std::endl;
    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
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
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery(
            "SELECT userId, keyword FROM keyword"
        ));
        
        while (response->next()) {
            unsigned int userId = response->getUInt("userId");
            std::string keyword = response->getString("keyword");
            
            result[userId].push_back(keyword);
        }
    } catch (sql::SQLException& exception) {
        std::cerr << "SQL Exception: " << exception.what() << std::endl;
    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
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
    } catch (sql::SQLException& exception) {
        std::cerr << "SQL Exception: " << exception.what() << std::endl;
    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
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
    } catch (sql::SQLException& exception) {
        std::cerr << "SQL Exception: " << exception.what() << std::endl;
    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
    }
    
    return false;
}
