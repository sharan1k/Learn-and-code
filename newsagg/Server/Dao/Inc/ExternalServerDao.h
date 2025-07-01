#pragma once

#include "../Inc/DbConnection.h"
#include "../../../Common/Inc/Dto/ExternalServer.h"
#include <memory>
#include <vector>
#include <string>

class ExternalServerDao {
public:
    ExternalServerDao();
    ~ExternalServerDao();
    
    bool createExternalServer(const ExternalServer& server);
    std::shared_ptr<ExternalServer> findById(unsigned int apiId);
    std::shared_ptr<ExternalServer> findByName(const std::string& apiName);
    bool updateExternalServer(const ExternalServer& server);
    bool deleteExternalServer(unsigned int apiId);
    std::vector<std::shared_ptr<ExternalServer>> getAllExternalServers();
    bool updateLastAccessed(unsigned int apiId, const std::string& timestamp);
    bool setApiStatus(unsigned int apiId, ApiStatus status);
};
