#include "../Inc/CategoryDao.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>

CategoryDao::CategoryDao() {
}

CategoryDao::~CategoryDao() {
}

bool CategoryDao::createCategory(const Category& category) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO category (categoryName) VALUES (?)"
        ));
        
        pstmt->setString(1, category.categoryName);
        int rowsAffected = pstmt->executeUpdate();
        
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in createCategory: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in createCategory: " << exception.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Category> CategoryDao::findById(unsigned int categoryId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM category WHERE categoryId = ?"
        ));
        
        pstmt->setUInt(1, categoryId);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        if (response->next()) {
            auto category = std::make_shared<Category>();
            category->categoryId = response->getUInt("categoryId");
            category->categoryName = response->getString("categoryName");
            return category;
        }
        
        return nullptr;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in findById: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return nullptr;
    } catch (std::exception &exception) {
        std::cerr << "Exception in findById: " << exception.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<Category> CategoryDao::findByName(const std::string& categoryName) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM category WHERE categoryName = ?"
        ));
        
        pstmt->setString(1, categoryName);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        if (response->next()) {
            auto category = std::make_shared<Category>();
            category->categoryId = response->getUInt("categoryId");
            category->categoryName = response->getString("categoryName");
            return category;
        }
        
        return nullptr;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in findByName: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return nullptr;
    } catch (std::exception &exception) {
        std::cerr << "Exception in findByName: " << exception.what() << std::endl;
        return nullptr;
    }
}

std::vector<std::shared_ptr<Category>> CategoryDao::getAllCategories() {
    std::vector<std::shared_ptr<Category>> categories;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery(
            "SELECT c.* FROM category c "
            "WHERE NOT EXISTS (SELECT 1 FROM hiddenCategory hc WHERE hc.categoryId = c.categoryId) "
            "ORDER BY c.categoryName"
        ));
        
        while (response->next()) {
            auto category = std::make_shared<Category>();
            category->categoryId = response->getUInt("categoryId");
            category->categoryName = response->getString("categoryName");
            categories.push_back(category);
        }
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in getAllCategories: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
    } catch (std::exception &exception) {
        std::cerr << "Exception in getAllCategories: " << exception.what() << std::endl;
    }
    
    return categories;
}

bool CategoryDao::updateCategory(const Category& category) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "UPDATE category SET categoryName = ? WHERE categoryId = ?"
        ));
        
        pstmt->setString(1, category.categoryName);
        pstmt->setUInt(2, category.categoryId);
        int rowsAffected = pstmt->executeUpdate();
        
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in updateCategory: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in updateCategory: " << exception.what() << std::endl;
        return false;
    }
}

bool CategoryDao::deleteCategory(unsigned int categoryId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "DELETE FROM category WHERE categoryId = ?"
        ));
        
        pstmt->setUInt(1, categoryId);
        int rowsAffected = pstmt->executeUpdate();
        
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in deleteCategory: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in deleteCategory: " << exception.what() << std::endl;
        return false;
    }
}

unsigned int CategoryDao::findOrCreateCategory(const std::string& categoryName) {
    auto existingCategory = findByName(categoryName);
    if (existingCategory) {
        return existingCategory->categoryId;
    }
    
    Category newCategory;
    newCategory.categoryName = categoryName;
    
    if (createCategory(newCategory)) {
        auto createdCategory = findByName(categoryName);
        if (createdCategory) {
            return createdCategory->categoryId;
        }
    }
    
    return 0;
}
