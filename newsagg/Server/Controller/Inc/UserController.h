#pragma once

#include "../Inc/User.h"
#include "../../Dao/Inc/UserDao.h"
#include "../../Http/Inc/HttpServer.h"
#include <functional>

class UserController {
public:
    static void registerRoutes(HttpServer& server);

private:
    static void handleSignup(const httplib::Request& req, httplib::Response& res);
    static void handleLogin(const httplib::Request& req, httplib::Response& res);
};
