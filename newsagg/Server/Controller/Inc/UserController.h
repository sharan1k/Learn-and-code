#pragma once

#include "../../Dto/Inc/User.h"
#include "../../Service/Inc/UserService.h"
#include "../../Http/Inc/HttpServer.h"
#include <functional>

class UserController {
public:
    static void registerRoutes(HttpServer& server);

private:
    static void handleSignup(const httplib::Request& req, httplib::Response& res);
    static void handleLogin(const httplib::Request& req, httplib::Response& res);
    static UserService& getUserService();
};
