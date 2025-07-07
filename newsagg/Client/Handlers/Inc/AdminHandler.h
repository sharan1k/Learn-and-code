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
    using ServerListCallback = std::function<void(bool success, const std::string& message, const std::vector<ExternalServer>& servers)>;
    
    using ServerDetailsCallback = std::function<void(bool success, const std::string& message, const ExternalServer& server)>;
    
    using StatusCallback = std::function<void(bool success, const std::string& message)>;
    
    explicit AdminHandler(std::shared_ptr<HttpClient> httpClient);
    
    void getExternalServers(ServerListCallback callback);
    
    void getExternalServerDetails(int serverId, ServerDetailsCallback callback);
    
    void updateExternalServer(const ExternalServer& server, StatusCallback callback);
    
    void addCategory(const std::string& categoryName, StatusCallback callback);

private:
    std::shared_ptr<HttpClient> client;
    
    std::string extractErrorMessage(
        const httplib::Result& result,
        const std::string& defaultMessage,
        const std::map<int, std::string>& statusCodes = {});
    
    nlohmann::json parseResponse(
        const httplib::Result& result,
        bool& success,
        std::string& errorMessage);
};

#endif // ADMIN_HANDLER_H
