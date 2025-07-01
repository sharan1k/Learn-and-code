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
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in createExternalServer: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Exception in createExternalServer: " << e.what() << std::endl;
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
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            auto server = std::make_shared<ExternalServer>();
            server->apiId = res->getUInt("apiId");
            server->apiName = res->getString("apiName");
            server->apiStatus = (res->getString("apiStatus") == "active") ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE;
            server->lastAccessed = res->getString("lastAccessed");
            server->apiKey = res->getString("apiKey");
            return server;
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

std::shared_ptr<ExternalServer> ExternalServerDao::findByName(const std::string& apiName) {
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
            "SELECT * FROM externalServer WHERE apiName = ?"
        ));
        
        pstmt->setString(1, apiName);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            auto server = std::make_shared<ExternalServer>();
            server->apiId = res->getUInt("apiId");
            server->apiName = res->getString("apiName");
            server->apiStatus = (res->getString("apiStatus") == "active") ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE;
            server->lastAccessed = res->getString("lastAccessed");
            server->apiKey = res->getString("apiKey");
            return server;
        }
        
        return nullptr;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in findByName: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return nullptr;
    } catch (std::exception &e) {
        std::cerr << "Exception in findByName: " << e.what() << std::endl;
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
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in updateExternalServer: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Exception in updateExternalServer: " << e.what() << std::endl;
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
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in deleteExternalServer: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Exception in deleteExternalServer: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<ExternalServer>> ExternalServerDao::getAllExternalServers() {
    std::vector<std::shared_ptr<ExternalServer>> servers;
    
    try {
        auto dbInstance = DbConnection::getInstance();
        auto conn = dbInstance->getConnection();
        
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM externalServer"));
        
        while (res->next()) {
            auto server = std::make_shared<ExternalServer>();
            server->apiId = res->getUInt("apiId");
            server->apiName = res->getString("apiName");
            server->apiStatus = (res->getString("apiStatus") == "active") ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE;
            server->lastAccessed = res->getString("lastAccessed");
            server->apiKey = res->getString("apiKey");
            servers.push_back(server);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in getAllExternalServers: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Exception in getAllExternalServers: " << e.what() << std::endl;
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
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in updateLastAccessed: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Exception in updateLastAccessed: " << e.what() << std::endl;
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
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException in setApiStatus: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return false;
    } catch (std::exception &e) {
        std::cerr << "Exception in setApiStatus: " << e.what() << std::endl;
        return false;
    }
}
