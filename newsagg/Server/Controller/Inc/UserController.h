#pragma once

#include "../../../Common/Inc/Dto/User.h"
#include "../../Service/Inc/UserService.h"
#include "../../Http/Inc/HttpServer.h"
#include <functional>
#include <nlohmann/json.hpp>

class UserController {
public:
    static void registerRoutes(HttpServer& server);

private:
    static void handleSignup(const httplib::Request& request, httplib::Response& response);
    static void handleLogin(const httplib::Request& request, httplib::Response& response);
    static UserService& getUserService();
    
    static void sendSuccessResponse(httplib::Response& response, const nlohmann::json& data, int status = 200, const std::string& message = "");
    static void sendErrorResponse(httplib::Response& response, const std::string& message, int status = 500);
};
