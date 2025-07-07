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

bool ArticleDao::createArticle(const Article& article, unsigned int* outArticleId) {
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
        
        if (outArticleId != nullptr && rowsAffected > 0) {
            std::unique_ptr<sql::Statement> stmt(conn->createStatement());
            std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT LAST_INSERT_ID()"));
            
            if (res->next()) {
                *outArticleId = res->getUInt(1);
            }
        }
        
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

std::vector<std::shared_ptr<Article>> ArticleDao::getArticlesByDate(const std::string& date, int limit) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM article WHERE publishedAt LIKE ? ORDER BY publishedAt DESC LIMIT ?"
        ));
        
        pstmt->setString(1, date + "%");
        pstmt->setInt(2, limit);
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
        std::cerr << "SQLException in getArticlesByDate: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in getArticlesByDate: " << e.what() << std::endl;
    }
    
    return articles;
}

std::vector<std::shared_ptr<Article>> ArticleDao::getArticlesByDateRange(
    const std::string& startDate, const std::string& endDate, int limit) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM article WHERE publishedAt >= ? AND publishedAt <= ? "
            "ORDER BY publishedAt DESC LIMIT ?"
        ));
        
        pstmt->setString(1, startDate); 
        pstmt->setString(2, endDate + " 23:59:59"); 
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
            article->publishedAt = res->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in getArticlesByDateRange: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in getArticlesByDateRange: " << e.what() << std::endl;
    }
    
    return articles;
}

std::vector<std::shared_ptr<Article>> ArticleDao::getArticlesByDateAndCategory(
    const std::string& date, unsigned int categoryId, int limit) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM article WHERE publishedAt LIKE ? AND categoryId = ? "
            "ORDER BY publishedAt DESC LIMIT ?"
        ));
        
        pstmt->setString(1, date + "%"); 
        pstmt->setUInt(2, categoryId);
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
            article->publishedAt = res->getString("publishedAt");
            articles.push_back(article);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in getArticlesByDateAndCategory: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in getArticlesByDateAndCategory: " << e.what() << std::endl;
    }
    
    return articles;
}

std::vector<std::shared_ptr<Article>> ArticleDao::getArticlesByDateRangeAndCategory(
    const std::string& startDate, const std::string& endDate, 
    unsigned int categoryId, int limit) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM article WHERE publishedAt >= ? AND publishedAt <= ? "
            "AND categoryId = ? ORDER BY publishedAt DESC LIMIT ?"
        ));
        
        pstmt->setString(1, startDate); 
        pstmt->setString(2, endDate + " 23:59:59"); 
        pstmt->setUInt(3, categoryId);
        pstmt->setInt(4, limit);
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
        std::cerr << "SQLException in getArticlesByDateRangeAndCategory: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in getArticlesByDateRangeAndCategory: " << e.what() << std::endl;
    }
    
    return articles;
}

std::vector<std::shared_ptr<Article>> ArticleDao::searchArticles(const std::string& query, int limit) {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::string likePattern = "%" + query + "%";
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT a.*, c.categoryName as categoryName FROM article a "
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
            "SELECT a.*, c.categoryName as categoryName FROM article a "
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
            "SELECT a.*, c.categoryName as categoryName, "
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
            "SELECT a.*, c.categoryName as categoryName, "
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
