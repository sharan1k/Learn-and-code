#include "../Inc/HiddenArticleDao.h"
#include "../../Utils/Inc/Logger.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

HiddenArticleDao::HiddenArticleDao() {}
HiddenArticleDao::~HiddenArticleDao() {}

bool HiddenArticleDao::hideArticle(unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        if (isArticleHidden(articleId)) {
            Logger::info("Article " + std::to_string(articleId) + " is already hidden");
            return true;
        }
        
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "INSERT INTO hiddenArticle (articleId) VALUES (?)"
        ));
        
        stmt->setUInt(1, articleId);
        
        int result = stmt->executeUpdate();
        Logger::info("Article " + std::to_string(articleId) + " has been hidden");
        return result > 0;
    } catch (sql::SQLException &exception) {
        Logger::error("SQLException in HiddenArticleDao::hideArticle: " + std::string(exception.what()));
        return false;
    } catch (std::exception &exception) {
        Logger::error("Exception in HiddenArticleDao::hideArticle: " + std::string(exception.what()));
        return false;
    }
}

bool HiddenArticleDao::unhideArticle(unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "DELETE FROM hiddenArticle WHERE articleId = ?"
        ));
        
        stmt->setUInt(1, articleId);
        
        int result = stmt->executeUpdate();
        Logger::info("Article " + std::to_string(articleId) + " has been unhidden");
        return result > 0;
    } catch (sql::SQLException &exception) {
        Logger::error("SQLException in HiddenArticleDao::unhideArticle: " + std::string(exception.what()));
        return false;
    } catch (std::exception &exception) {
        Logger::error("Exception in HiddenArticleDao::unhideArticle: " + std::string(exception.what()));
        return false;
    }
}

bool HiddenArticleDao::isArticleHidden(unsigned int articleId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT 1 FROM hiddenArticle WHERE articleId = ?"
        ));
        
        stmt->setUInt(1, articleId);
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery());
        
        return response->next();
    } catch (sql::SQLException &exception) {
        Logger::error("SQLException in HiddenArticleDao::isArticleHidden: " + std::string(exception.what()));
        return false;
    } catch (std::exception &exception) {
        Logger::error("Exception in HiddenArticleDao::isArticleHidden: " + std::string(exception.what()));
        return false;
    }
}

std::vector<unsigned int> HiddenArticleDao::getAllHiddenArticleIds() {
    std::vector<unsigned int> hiddenIds;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT articleId FROM hiddenArticle"
        ));
        
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery());
        
        while (response->next()) {
            hiddenIds.push_back(response->getUInt("articleId"));
        }
        
        Logger::debug("Retrieved " + std::to_string(hiddenIds.size()) + " hidden article IDs");
    } catch (sql::SQLException &exception) {
        Logger::error("SQLException in HiddenArticleDao::getAllHiddenArticleIds: " + std::string(exception.what()));
    } catch (std::exception &exception) {
        Logger::error("Exception in HiddenArticleDao::getAllHiddenArticleIds: " + std::string(exception.what()));
    }
    
    return hiddenIds;
}
