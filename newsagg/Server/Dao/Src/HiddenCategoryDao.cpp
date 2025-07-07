#include "../Inc/HiddenCategoryDao.h"
#include "../../Utils/Inc/Logger.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

HiddenCategoryDao::HiddenCategoryDao() {}
HiddenCategoryDao::~HiddenCategoryDao() {}

bool HiddenCategoryDao::hideCategory(unsigned int categoryId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        if (isCategoryHidden(categoryId)) {
            Logger::info("Category " + std::to_string(categoryId) + " is already hidden");
            return true;
        }
        
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "INSERT INTO hiddenCategory (categoryId) VALUES (?)"
        ));
        
        stmt->setUInt(1, categoryId);
        
        int result = stmt->executeUpdate();
        Logger::info("Category " + std::to_string(categoryId) + " has been hidden");
        return result > 0;
    } catch (sql::SQLException &exception) {
        Logger::error("SQLException in HiddenCategoryDao::hideCategory: " + std::string(exception.what()));
        return false;
    } catch (std::exception &exception) {
        Logger::error("Exception in HiddenCategoryDao::hideCategory: " + std::string(exception.what()));
        return false;
    }
}

bool HiddenCategoryDao::unhideCategory(unsigned int categoryId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "DELETE FROM hiddenCategory WHERE categoryId = ?"
        ));
        
        stmt->setUInt(1, categoryId);
        
        int result = stmt->executeUpdate();
        Logger::info("Category " + std::to_string(categoryId) + " has been unhidden");
        return result > 0;
    } catch (sql::SQLException &exception) {
        Logger::error("SQLException in HiddenCategoryDao::unhideCategory: " + std::string(exception.what()));
        return false;
    } catch (std::exception &exception) {
        Logger::error("Exception in HiddenCategoryDao::unhideCategory: " + std::string(exception.what()));
        return false;
    }
}

bool HiddenCategoryDao::isCategoryHidden(unsigned int categoryId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT 1 FROM hiddenCategory WHERE categoryId = ?"
        ));
        
        stmt->setUInt(1, categoryId);
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery());
        
        return response->next();
    } catch (sql::SQLException &exception) {
        Logger::error("SQLException in HiddenCategoryDao::isCategoryHidden: " + std::string(exception.what()));
        return false;
    } catch (std::exception &exception) {
        Logger::error("Exception in HiddenCategoryDao::isCategoryHidden: " + std::string(exception.what()));
        return false;
    }
}

std::vector<unsigned int> HiddenCategoryDao::getAllHiddenCategoryIds() {
    std::vector<unsigned int> hiddenIds;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        std::unique_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "SELECT categoryId FROM hiddenCategory"
        ));
        
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery());
        
        while (response->next()) {
            hiddenIds.push_back(response->getUInt("categoryId"));
        }
        
        Logger::debug("Retrieved " + std::to_string(hiddenIds.size()) + " hidden category IDs");
    } catch (sql::SQLException &exception) {
        Logger::error("SQLException in HiddenCategoryDao::getAllHiddenCategoryIds: " + std::string(exception.what()));
    } catch (std::exception &exception) {
        Logger::error("Exception in HiddenCategoryDao::getAllHiddenCategoryIds: " + std::string(exception.what()));
    }
    
    return hiddenIds;
}
