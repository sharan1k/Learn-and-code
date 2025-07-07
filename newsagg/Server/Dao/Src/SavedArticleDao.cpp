#include "../Inc/SavedArticleDao.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>

SavedArticleDao::SavedArticleDao() {
}

SavedArticleDao::~SavedArticleDao() {
}

bool SavedArticleDao::saveArticle(const SavedArticle& savedArticle) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        if (isArticleSavedByUser(savedArticle.userId, savedArticle.articleId)) {
            return true;
        }
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO savedArticle (userId, articleId) VALUES (?, ?)"
        ));
        
        pstmt->setUInt(1, savedArticle.userId);
        pstmt->setUInt(2, savedArticle.articleId);
        
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in saveArticle: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in saveArticle: " << exception.what() << std::endl;
        return false;
    }
}

bool SavedArticleDao::isArticleSavedByUser(unsigned int userId, unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT COUNT(*) as count FROM savedArticle WHERE userId = ? AND articleId = ?"
        ));
        
        pstmt->setUInt(1, userId);
        pstmt->setUInt(2, articleId);
        
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        if (response->next()) {
            return response->getUInt("count") > 0;
        }
        
        return false;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in isArticleSavedByUser: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in isArticleSavedByUser: " << exception.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<Article>> SavedArticleDao::getSavedArticlesByUser(unsigned int userId) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT a.* FROM article a "
            "INNER JOIN savedArticle sa ON a.articleId = sa.articleId "
            "WHERE sa.userId = ?"
        ));
        
        pstmt->setUInt(1, userId);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        while (response->next()) {
            auto article = std::make_shared<Article>();
            article->articleId = response->getUInt("articleId");
            article->title = response->getString("title");
            article->description = response->getString("description");
            article->source = response->getString("source");
            article->url = response->getString("url");
            article->categoryId = response->getUInt("categoryId");
            article->publishedAt = response->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in getSavedArticlesByUser: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
    } catch (std::exception &exception) {
        std::cerr << "Exception in getSavedArticlesByUser: " << exception.what() << std::endl;
    }
    
    return articles;
}

bool SavedArticleDao::updateSavedArticle(const SavedArticle& savedArticle) {
    try {
        return isArticleSavedByUser(savedArticle.userId, savedArticle.articleId);
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in updateSavedArticle: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in updateSavedArticle: " << exception.what() << std::endl;
        return false;
    }
}

bool SavedArticleDao::removeSavedArticle(unsigned int userId, unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "DELETE FROM savedArticle WHERE userId = ? AND articleId = ?"
        ));
        
        pstmt->setUInt(1, userId);
        pstmt->setUInt(2, articleId);
        
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in removeSavedArticle: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in removeSavedArticle: " << exception.what() << std::endl;
        return false;
    }
}
