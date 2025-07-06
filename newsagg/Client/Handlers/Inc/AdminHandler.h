#ifndef ADMIN_HANDLER_H
#define ADMIN_HANDLER_H

#include "../../Http/Inc/HttpClient.h"
#include "../../../Common/Inc/Dto/ExternalServer.h"
#include "../../../Common/Inc/Dto/Category.h"
#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <functional>
#include <vector>

class AdminHandler {
public:
    AdminHandler(std::shared_ptr<HttpClient> client);
    
    void getExternalServers(std::function<void(bool success, const std::string& message, const std::vector<ExternalServer>& servers)> callback);
    
    void getExternalServerDetails(int serverId, std::function<void(bool success, const std::string& message, const ExternalServer& server)> callback);
    
    void updateExternalServer(const ExternalServer& server, std::function<void(bool success, const std::string& message)> callback);
    
    void addCategory(const std::string& categoryName, std::function<void(bool success, const std::string& message)> callback);

private:
    std::shared_ptr<HttpClient> client;
};

#endif // ADMIN_HANDLER_H
