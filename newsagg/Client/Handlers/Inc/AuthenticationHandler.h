#ifndef AUTHENTICATION_HANDLER_H
#define AUTHENTICATION_HANDLER_H

#include "../../Http/Inc/HttpClient.h"
#include "../../../Common/Inc/Dto/User.h"
#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <functional>
#include <map>

class AuthenticationHandler {
public:
    using AuthCallback = std::function<void(bool success, const std::string& message, const User* user)>;
    
    explicit AuthenticationHandler(std::shared_ptr<HttpClient> httpClient);
    
    void login(const std::string& userName, 
               const std::string& password,
               AuthCallback callback);
               
    void signup(const std::string& userName,
                const std::string& email,
                const std::string& password,
                AuthCallback callback);

private:
    std::shared_ptr<HttpClient> client;
    User currentUser;
    
    std::string extractErrorMessage(
        const httplib::Result& result,
        const std::string& defaultMessage,
        const std::map<int, std::string>& statusCodes = {});
    
    nlohmann::json parseResponse(
        const httplib::Result& result,
        bool& success,
        std::string& errorMessage);
};

#endif // AUTHENTICATION_HANDLER_H
