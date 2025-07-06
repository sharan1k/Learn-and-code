#pragma once

#include "../../Dao/Inc/ExternalServerDao.h"
#include <memory>
#include <vector>
#include <string>

class ExternalServerService {
public:
    ExternalServerService();
    ~ExternalServerService();
    
    std::vector<std::shared_ptr<ExternalServer>> getAllExternalServers();
    std::shared_ptr<ExternalServer> getExternalServerById(unsigned int serverId);
    bool updateExternalServer(const ExternalServer& server);
    
private:
    ExternalServerDao serverDao;
};
