#include "../Inc/NotificationSettingDao.h"
#include "../Inc/DbConnection.h"
#include <iostream>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

std::vector<std::shared_ptr<NotificationSetting>> NotificationSettingDao::getByUser(unsigned int userId) {
    std::vector<std::shared_ptr<NotificationSetting>> settings;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return settings;
        }
        
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery(
            "SELECT userId, categoryId FROM notificationSettings WHERE userId = " + std::to_string(userId)
        ));
        
        while (response->next()) {
            auto setting = std::make_shared<NotificationSetting>();
            setting->userId = response->getUInt("userId");
            setting->categoryId = response->getUInt("categoryId");
            settings.push_back(setting);
        }
    } catch (sql::SQLException& exception) {
        std::cerr << "SQL Exception: " << exception.what() << std::endl;
    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
    }
    
    return settings;
}

std::vector<std::shared_ptr<NotificationSetting>> NotificationSettingDao::getByCategoryId(unsigned int categoryId) {
    std::vector<std::shared_ptr<NotificationSetting>> settings;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return settings;
        }
        
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery(
            "SELECT userId, categoryId FROM notificationSettings WHERE categoryId = " + std::to_string(categoryId)
        ));
        
        while (response->next()) {
            auto setting = std::make_shared<NotificationSetting>();
            setting->userId = response->getUInt("userId");
            setting->categoryId = response->getUInt("categoryId");
            settings.push_back(setting);
        }
    } catch (sql::SQLException& exception) {
        std::cerr << "SQL Exception: " << exception.what() << std::endl;
    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
    }
    
    return settings;
}

bool NotificationSettingDao::add(const NotificationSetting& setting) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return false;
        }
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO notificationSettings (userId, categoryId) VALUES (?, ?)"
        ));
        
        pstmt->setUInt(1, setting.userId);
        pstmt->setUInt(2, setting.categoryId);
        
        int affectedRows = pstmt->executeUpdate();
        return affectedRows > 0;
    } catch (sql::SQLException& exception) {
        std::cerr << "SQL Exception: " << exception.what() << std::endl;
    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
    }
    
    return false;
}

bool NotificationSettingDao::remove(unsigned int userId, unsigned int categoryId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return false;
        }
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "DELETE FROM notificationSettings WHERE userId = ? AND categoryId = ?"
        ));
        
        pstmt->setUInt(1, userId);
        pstmt->setUInt(2, categoryId);
        
        int affectedRows = pstmt->executeUpdate();
        return affectedRows > 0;
    } catch (sql::SQLException& exception) {
        std::cerr << "SQL Exception: " << exception.what() << std::endl;
    } catch (std::exception& exception) {
        std::cerr << "Exception: " << exception.what() << std::endl;
    }
    
    return false;
}
