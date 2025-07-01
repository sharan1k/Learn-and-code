#include "../Inc/ArticleDao.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>

ArticleDao::ArticleDao() {
}

ArticleDao::~ArticleDao() {
}

bool ArticleDao::createArticle(const Article& article) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO article (title, description, source, url, categoryId, publishedAt) VALUES (?, ?, ?, ?, ?, ?)"
        ));
        
        pstmt->setString(1, article.title);
        pstmt->setString(2, article.description);
        pstmt->setString(3, article.source);
        pstmt->setString(4, article.url);
        pstmt->setUInt(5, article.categoryId);
        pstmt->setString(6, article.publishedAt);
        
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in createArticle: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Exception in createArticle: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Article> ArticleDao::findById(unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM article WHERE articleId = ?"
        ));
        
        pstmt->setUInt(1, articleId);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            auto article = std::make_shared<Article>();
            article->articleId = res->getUInt("articleId");
            article->title = res->getString("title");
            article->description = res->getString("description");
            article->source = res->getString("source");
            article->url = res->getString("url");
            article->categoryId = res->getUInt("categoryId");
            article->publishedAt = res->getString("publishedAt");
            return article;
        }
        
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in findById: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return nullptr;
    } catch (std::exception &e) {
        std::cerr << "Exception in findById: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<std::shared_ptr<Article>> ArticleDao::findByCategory(unsigned int categoryId) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM article WHERE categoryId = ? ORDER BY publishedAt DESC"
        ));
        
        pstmt->setUInt(1, categoryId);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        while (res->next()) {
            auto article = std::make_shared<Article>();
            article->articleId = res->getUInt("articleId");
            article->title = res->getString("title");
            article->description = res->getString("description");
            article->source = res->getString("source");
            article->url = res->getString("url");
            article->categoryId = res->getUInt("categoryId");
            article->publishedAt = res->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in findByCategory: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in findByCategory: " << e.what() << std::endl;
    }
    
    return articles;
}

std::vector<std::shared_ptr<Article>> ArticleDao::getLatestArticles(int limit) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM article ORDER BY publishedAt DESC LIMIT ?"
        ));
        
        pstmt->setInt(1, limit);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        while (res->next()) {
            auto article = std::make_shared<Article>();
            article->articleId = res->getUInt("articleId");
            article->title = res->getString("title");
            article->description = res->getString("description");
            article->source = res->getString("source");
            article->url = res->getString("url");
            article->categoryId = res->getUInt("categoryId");
            article->publishedAt = res->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in getLatestArticles: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in getLatestArticles: " << e.what() << std::endl;
    }
    
    return articles;
}

std::vector<std::shared_ptr<Article>> ArticleDao::findBySource(const std::string& source) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM article WHERE source = ? ORDER BY publishedAt DESC"
        ));
        
        pstmt->setString(1, source);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        while (res->next()) {
            auto article = std::make_shared<Article>();
            article->articleId = res->getUInt("articleId");
            article->title = res->getString("title");
            article->description = res->getString("description");
            article->source = res->getString("source");
            article->url = res->getString("url");
            article->categoryId = res->getUInt("categoryId");
            article->publishedAt = res->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in findBySource: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in findBySource: " << e.what() << std::endl;
    }
    
    return articles;
}

bool ArticleDao::deleteArticle(unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "DELETE FROM article WHERE articleId = ?"
        ));
        
        pstmt->setUInt(1, articleId);
        int rowsAffected = pstmt->executeUpdate();
        
        return rowsAffected > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in deleteArticle: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Exception in deleteArticle: " << e.what() << std::endl;
        return false;
    }
}

bool ArticleDao::updateArticle(const Article& article) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "UPDATE article SET title = ?, description = ?, source = ?, url = ?, categoryId = ?, publishedAt = ? WHERE articleId = ?"
        ));
        
        pstmt->setString(1, article.title);
        pstmt->setString(2, article.description);
        pstmt->setString(3, article.source);
        pstmt->setString(4, article.url);
        pstmt->setUInt(5, article.categoryId);
        pstmt->setString(6, article.publishedAt);
        pstmt->setUInt(7, article.articleId);
        
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in updateArticle: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Exception in updateArticle: " << e.what() << std::endl;
        return false;
    }
}

bool ArticleDao::articleExists(const std::string& url) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT COUNT(*) as count FROM article WHERE url = ?"
        ));
        
        pstmt->setString(1, url);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            return res->getUInt("count") > 0;
        }
        
        return false;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in articleExists: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Exception in articleExists: " << e.what() << std::endl;
        return false;
    }
}
