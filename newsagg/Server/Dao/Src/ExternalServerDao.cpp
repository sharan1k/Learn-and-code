#include "../Inc/ExternalServerDao.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>

ExternalServerDao::ExternalServerDao() {
}

ExternalServerDao::~ExternalServerDao() {
}

bool ExternalServerDao::createExternalServer(const ExternalServer& server) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "INSERT INTO externalServer (apiName, apiStatus, lastAccessed, apiKey) VALUES (?, ?, ?, ?)"
        ));
        
        pstmt->setString(1, server.apiName);
        pstmt->setString(2, (server.apiStatus == ApiStatus::ACTIVE) ? "active" : "notActive");
        pstmt->setString(3, server.lastAccessed);
        pstmt->setString(4, server.apiKey);
        
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in createExternalServer: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in createExternalServer: " << exception.what() << std::endl;
        return false;
    }
}

std::shared_ptr<ExternalServer> ExternalServerDao::findById(unsigned int apiId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM externalServer WHERE apiId = ?"
        ));
        
        pstmt->setUInt(1, apiId);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        if (response->next()) {
            auto server = std::make_shared<ExternalServer>();
            server->apiId = response->getUInt("apiId");
            server->apiName = response->getString("apiName");
            server->apiStatus = (response->getString("apiStatus") == "active") ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE;
            server->lastAccessed = response->getString("lastAccessed");
            server->apiKey = response->getString("apiKey");
            return server;
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

std::shared_ptr<ExternalServer> ExternalServerDao::findByName(const std::string& apiName) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM externalServer WHERE apiName = ?"
        ));
        
        pstmt->setString(1, apiName);
        std::unique_ptr<sql::ResultSet> response(pstmt->executeQuery());
        
        if (response->next()) {
            auto server = std::make_shared<ExternalServer>();
            server->apiId = response->getUInt("apiId");
            server->apiName = response->getString("apiName");
            server->apiStatus = (response->getString("apiStatus") == "active") ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE;
            server->lastAccessed = response->getString("lastAccessed");
            server->apiKey = response->getString("apiKey");
            return server;
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

bool ExternalServerDao::updateExternalServer(const ExternalServer& server) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "UPDATE externalServer SET apiName = ?, apiStatus = ?, lastAccessed = ?, apiKey = ? WHERE apiId = ?"
        ));
        
        pstmt->setString(1, server.apiName);
        pstmt->setString(2, (server.apiStatus == ApiStatus::ACTIVE) ? "active" : "notActive");
        pstmt->setString(3, server.lastAccessed);
        pstmt->setString(4, server.apiKey);
        pstmt->setUInt(5, server.apiId);
        
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in updateExternalServer: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in updateExternalServer: " << exception.what() << std::endl;
        return false;
    }
}

bool ExternalServerDao::deleteExternalServer(unsigned int apiId) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "DELETE FROM externalServer WHERE apiId = ?"
        ));
        
        pstmt->setUInt(1, apiId);
        int rowsAffected = pstmt->executeUpdate();
        
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in deleteExternalServer: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in deleteExternalServer: " << exception.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<ExternalServer>> ExternalServerDao::getAllExternalServers() {
    std::vector<std::shared_ptr<ExternalServer>> servers;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> response(stmt->executeQuery("SELECT * FROM externalServer"));
        
        while (response->next()) {
            auto server = std::make_shared<ExternalServer>();
            server->apiId = response->getUInt("apiId");
            server->apiName = response->getString("apiName");
            server->apiStatus = (response->getString("apiStatus") == "active") ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE;
            server->lastAccessed = response->getString("lastAccessed");
            server->apiKey = response->getString("apiKey");
            servers.push_back(server);
        }
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in getAllExternalServers: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
    } catch (std::exception &exception) {
        std::cerr << "Exception in getAllExternalServers: " << exception.what() << std::endl;
    }
    
    return servers;
}

bool ExternalServerDao::updateLastAccessed(unsigned int apiId, const std::string& timestamp) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "UPDATE externalServer SET lastAccessed = ? WHERE apiId = ?"
        ));
        
        pstmt->setString(1, timestamp);
        pstmt->setUInt(2, apiId);
        
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in updateLastAccessed: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in updateLastAccessed: " << exception.what() << std::endl;
        return false;
    }
}

bool ExternalServerDao::setApiStatus(unsigned int apiId, ApiStatus status) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "UPDATE externalServer SET apiStatus = ? WHERE apiId = ?"
        ));
        
        pstmt->setString(1, (status == ApiStatus::ACTIVE) ? "active" : "notActive");
        pstmt->setUInt(2, apiId);
        
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (sql::SQLException &exception) {
        std::cerr << "SQLException in setApiStatus: " << exception.what() << std::endl;
        std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
        return false;
    } catch (std::exception &exception) {
        std::cerr << "Exception in setApiStatus: " << exception.what() << std::endl;
        return false;
    }
}
