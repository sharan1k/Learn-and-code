#include "../Inc/ReportedArticleDao.h"
#include "../Inc/ArticleDao.h"
#include "../../Utils/Inc/Logger.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <sstream>

ReportedArticleDao::ReportedArticleDao() {}
ReportedArticleDao::~ReportedArticleDao() {}

bool ReportedArticleDao::reportArticle(unsigned int userId, unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "INSERT INTO reportedArticle (userId, articleId, seenStatus) "
            "VALUES (?, ?, 'unseen') "
            "ON DUPLICATE KEY UPDATE seenStatus = 'unseen'"
        ));
        
        stmt->setUInt(1, userId);
        stmt->setUInt(2, articleId);
        
        int result = stmt->executeUpdate();
        Logger::info("Article " + std::to_string(articleId) + " reported by user " + std::to_string(userId));
        return result > 0;
    } catch (sql::SQLException &e) {
        Logger::error("SQLException in ReportedArticleDao::reportArticle: " + std::string(e.what()));
        return false;
    } catch (std::exception &e) {
        Logger::error("Exception in ReportedArticleDao::reportArticle: " + std::string(e.what()));
        return false;
    }
}

bool ReportedArticleDao::markReportAsSeen(unsigned int userId, unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "UPDATE reportedArticle SET seenStatus = 'seen' WHERE userId = ? AND articleId = ?"
        ));
        
        stmt->setUInt(1, userId);
        stmt->setUInt(2, articleId);
        
        int result = stmt->executeUpdate();
        return result > 0;
    } catch (sql::SQLException &e) {
        Logger::error("SQLException in ReportedArticleDao::markReportAsSeen: " + std::string(e.what()));
        return false;
    } catch (std::exception &e) {
        Logger::error("Exception in ReportedArticleDao::markReportAsSeen: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::shared_ptr<Article>> ReportedArticleDao::getUnseenReportedArticles() {
    std::vector<std::shared_ptr<Article>> articles;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT DISTINCT a.* FROM article a "
            "JOIN reportedArticle ra ON a.articleId = ra.articleId "
            "WHERE ra.seenStatus = 'unseen'"
        ));
        
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        
        ArticleDao articleDao;
        
        while (res->next()) {
            std::shared_ptr<Article> article = std::make_shared<Article>();
            article->articleId = res->getUInt("articleId");
            article->title = res->getString("title");
            article->description = res->getString("description");
            article->source = res->getString("source");
            article->url = res->getString("url");
            article->categoryId = res->getUInt("categoryId");
            article->publishedAt = res->getString("publishedAt");
            
            articles.push_back(article);
        }
        
        Logger::info("Retrieved " + std::to_string(articles.size()) + " unseen reported articles");
    } catch (sql::SQLException &e) {
        Logger::error("SQLException in ReportedArticleDao::getUnseenReportedArticles: " + std::string(e.what()));
    } catch (std::exception &e) {
        Logger::error("Exception in ReportedArticleDao::getUnseenReportedArticles: " + std::string(e.what()));
    }
    
    return articles;
}

int ReportedArticleDao::getReportCountForArticle(unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT COUNT(*) as reportCount FROM reportedArticle WHERE articleId = ?"
        ));
        
        stmt->setUInt(1, articleId);
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
        
        if (res->next()) {
            return res->getInt("reportCount");
        }
        
        return 0;
    } catch (sql::SQLException &e) {
        Logger::error("SQLException in ReportedArticleDao::getReportCountForArticle: " + std::string(e.what()));
        return 0;
    } catch (std::exception &e) {
        Logger::error("Exception in ReportedArticleDao::getReportCountForArticle: " + std::string(e.what()));
        return 0;
    }
}
