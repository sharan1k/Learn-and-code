#include "../Inc/NotificationDao.h"
#include "../Inc/DbConnection.h"
#include <iostream>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

std::vector<std::shared_ptr<Notification>> NotificationDao::getNotificationsByUser(unsigned int userId) {
    std::vector<std::shared_ptr<Notification>> notifications;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return notifications;
        }
        
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(
            "SELECT userId, articleId, seenStatus FROM notification WHERE userId = " + std::to_string(userId)
        ));
        
        while (res->next()) {
            auto notification = std::make_shared<Notification>();
            notification->userId = res->getUInt("userId");
            notification->articleId = res->getUInt("articleId");
            notification->seenStatus = res->getString("seenStatus");
            notifications.push_back(notification);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return notifications;
}

bool NotificationDao::add(const Notification& notification) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return false;
        }
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO notification (userId, articleId, seenStatus) VALUES (?, ?, ?)"
        ));
        
        pstmt->setUInt(1, notification.userId);
        pstmt->setUInt(2, notification.articleId);
        pstmt->setString(3, notification.seenStatus);
        
        int affectedRows = pstmt->executeUpdate();
        return affectedRows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return false;
}

bool NotificationDao::markAllAsSeen(unsigned int userId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return false;
        }
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "UPDATE notification SET seenStatus = 'seen' WHERE userId = ? AND seenStatus = 'unseen'"
        ));
        
        pstmt->setUInt(1, userId);
        
        int affectedRows = pstmt->executeUpdate();
        return true; 
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return false;
}

bool NotificationDao::remove(unsigned int userId, unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        if (!conn) {
            std::cerr << "Database connection not available" << std::endl;
            return false;
        }
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "DELETE FROM notification WHERE userId = ? AND articleId = ?"
        ));
        
        pstmt->setUInt(1, userId);
        pstmt->setUInt(2, articleId);
        
        int affectedRows = pstmt->executeUpdate();
        return affectedRows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Exception: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return false;
}
