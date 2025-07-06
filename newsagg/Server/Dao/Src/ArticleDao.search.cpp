#include "../Inc/ArticleDao.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>

std::vector<std::shared_ptr<Article>> ArticleDao::searchArticles(const std::string& query, int limit) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::string likePattern = "%" + query + "%";
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT a.*, c.name as categoryName FROM article a "
            "LEFT JOIN category c ON a.categoryId = c.categoryId "
            "WHERE a.title LIKE ? OR a.description LIKE ? "
            "ORDER BY a.publishedAt DESC LIMIT ?"
        ));
        
        pstmt->setString(1, likePattern);
        pstmt->setString(2, likePattern);
        pstmt->setInt(3, limit);
        
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        while (res->next()) {
            auto article = std::make_shared<Article>();
            article->articleId = res->getUInt("articleId");
            article->title = res->getString("title");
            article->description = res->getString("description");
            article->source = res->getString("source");
            article->url = res->getString("url");
            article->categoryId = res->getUInt("categoryId");
            article->category = res->getString("categoryName");
            article->publishedAt = res->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in searchArticles: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in searchArticles: " << e.what() << std::endl;
    }
    
    return articles;
}

std::vector<std::shared_ptr<Article>> ArticleDao::searchArticlesByDateRange(
    const std::string& query, const std::string& startDate, const std::string& endDate, int limit) {
    
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::string likePattern = "%" + query + "%";
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT a.*, c.name as categoryName FROM article a "
            "LEFT JOIN category c ON a.categoryId = c.categoryId "
            "WHERE (a.title LIKE ? OR a.description LIKE ?) "
            "AND DATE(a.publishedAt) BETWEEN ? AND ? "
            "ORDER BY a.publishedAt DESC LIMIT ?"
        ));
        
        pstmt->setString(1, likePattern);
        pstmt->setString(2, likePattern);
        pstmt->setString(3, startDate);
        pstmt->setString(4, endDate);
        pstmt->setInt(5, limit);
        
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        while (res->next()) {
            auto article = std::make_shared<Article>();
            article->articleId = res->getUInt("articleId");
            article->title = res->getString("title");
            article->description = res->getString("description");
            article->source = res->getString("source");
            article->url = res->getString("url");
            article->categoryId = res->getUInt("categoryId");
            article->category = res->getString("categoryName");
            article->publishedAt = res->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in searchArticlesByDateRange: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in searchArticlesByDateRange: " << e.what() << std::endl;
    }
    
    return articles;
}

std::vector<std::shared_ptr<Article>> ArticleDao::searchArticlesSortedByLikes(
    const std::string& query, bool sortByLikes, bool isDescending, int limit) {
    
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::string likePattern = "%" + query + "%";
        
        std::string sortSql;
        if (sortByLikes) {
            sortSql = isDescending ? 
                "ORDER BY COUNT(CASE WHEN l.likeDislike = 'like' THEN 1 END) DESC, a.publishedAt DESC" :
                "ORDER BY COUNT(CASE WHEN l.likeDislike = 'like' THEN 1 END) ASC, a.publishedAt DESC";
        } else {
            sortSql = isDescending ? 
                "ORDER BY COUNT(CASE WHEN l.likeDislike = 'dislike' THEN 1 END) DESC, a.publishedAt DESC" :
                "ORDER BY COUNT(CASE WHEN l.likeDislike = 'dislike' THEN 1 END) ASC, a.publishedAt DESC";
        }
        
        std::string sql = 
            "SELECT a.*, c.name as categoryName, "
            "COUNT(CASE WHEN l.likeDislike = 'like' THEN 1 END) as likeCount, "
            "COUNT(CASE WHEN l.likeDislike = 'dislike' THEN 1 END) as dislikeCount "
            "FROM article a "
            "LEFT JOIN category c ON a.categoryId = c.categoryId "
            "LEFT JOIN likes l ON a.articleId = l.articleId "
            "WHERE a.title LIKE ? OR a.description LIKE ? "
            "GROUP BY a.articleId " + sortSql + " LIMIT ?";
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(sql));
        
        pstmt->setString(1, likePattern);
        pstmt->setString(2, likePattern);
        pstmt->setInt(3, limit);
        
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        while (res->next()) {
            auto article = std::make_shared<Article>();
            article->articleId = res->getUInt("articleId");
            article->title = res->getString("title");
            article->description = res->getString("description");
            article->source = res->getString("source");
            article->url = res->getString("url");
            article->categoryId = res->getUInt("categoryId");
            article->category = res->getString("categoryName");
            article->publishedAt = res->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in searchArticlesSortedByLikes: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in searchArticlesSortedByLikes: " << e.what() << std::endl;
    }
    
    return articles;
}

std::vector<std::shared_ptr<Article>> ArticleDao::searchArticlesByDateRangeSortedByLikes(
    const std::string& query, const std::string& startDate, const std::string& endDate,
    bool sortByLikes, bool isDescending, int limit) {
    
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::string likePattern = "%" + query + "%";
        
        std::string sortSql;
        if (sortByLikes) {
            sortSql = isDescending ? 
                "ORDER BY COUNT(CASE WHEN l.likeDislike = 'like' THEN 1 END) DESC, a.publishedAt DESC" :
                "ORDER BY COUNT(CASE WHEN l.likeDislike = 'like' THEN 1 END) ASC, a.publishedAt DESC";
        } else {
            sortSql = isDescending ? 
                "ORDER BY COUNT(CASE WHEN l.likeDislike = 'dislike' THEN 1 END) DESC, a.publishedAt DESC" :
                "ORDER BY COUNT(CASE WHEN l.likeDislike = 'dislike' THEN 1 END) ASC, a.publishedAt DESC";
        }
        
        std::string sql = 
            "SELECT a.*, c.name as categoryName, "
            "COUNT(CASE WHEN l.likeDislike = 'like' THEN 1 END) as likeCount, "
            "COUNT(CASE WHEN l.likeDislike = 'dislike' THEN 1 END) as dislikeCount "
            "FROM article a "
            "LEFT JOIN category c ON a.categoryId = c.categoryId "
            "LEFT JOIN likes l ON a.articleId = l.articleId "
            "WHERE (a.title LIKE ? OR a.description LIKE ?) "
            "AND DATE(a.publishedAt) BETWEEN ? AND ? "
            "GROUP BY a.articleId " + sortSql + " LIMIT ?";
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(sql));
        
        pstmt->setString(1, likePattern);
        pstmt->setString(2, likePattern);
        pstmt->setString(3, startDate);
        pstmt->setString(4, endDate);
        pstmt->setInt(5, limit);
        
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        while (res->next()) {
            auto article = std::make_shared<Article>();
            article->articleId = res->getUInt("articleId");
            article->title = res->getString("title");
            article->description = res->getString("description");
            article->source = res->getString("source");
            article->url = res->getString("url");
            article->categoryId = res->getUInt("categoryId");
            article->category = res->getString("categoryName");
            article->publishedAt = res->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in searchArticlesByDateRangeSortedByLikes: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in searchArticlesByDateRangeSortedByLikes: " << e.what() << std::endl;
    }
    
    return articles;
}
