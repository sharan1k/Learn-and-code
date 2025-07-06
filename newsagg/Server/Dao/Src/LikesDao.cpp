#include "../Inc/LikesDao.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>

LikesDao::LikesDao() {
}

LikesDao::~LikesDao() {
}

bool LikesDao::addLike(unsigned int userId, unsigned int articleId, const std::string& likeDislike) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        if (hasUserLiked(userId, articleId)) {
            std::unique_ptr<sql::PreparedStatement> deleteStmt(conn->prepareStatement(
                "DELETE FROM likes WHERE userId = ? AND articleId = ?"
            ));
            deleteStmt->setUInt(1, userId);
            deleteStmt->setUInt(2, articleId);
            deleteStmt->executeUpdate();
        }
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO likes (userId, articleId, likeDislike) VALUES (?, ?, ?)"
        ));
        pstmt->setUInt(1, userId);
        pstmt->setUInt(2, articleId);
        pstmt->setString(3, likeDislike);
        
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in addLike: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        std::cerr << "ErrorCode: " << e.getErrorCode() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Error in addLike: " << e.what() << std::endl;
        return false;
    }
}

bool LikesDao::hasUserLiked(unsigned int userId, unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT COUNT(*) AS count FROM likes WHERE userId = ? AND articleId = ?"
        ));
        pstmt->setUInt(1, userId);
        pstmt->setUInt(2, articleId);
        
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            int count = res->getInt("count");
            return count > 0;
        }
        
        return false;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in hasUserLiked: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        std::cerr << "ErrorCode: " << e.getErrorCode() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Error in hasUserLiked: " << e.what() << std::endl;
        return false;
    }
}




