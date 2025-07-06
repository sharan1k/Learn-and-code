#pragma once

#include "../../../Common/Inc/Dto/ExternalServer.h"
#include "../../../Common/Inc/Dto/Category.h"
#include "../../Http/Inc/HttpServer.h"
#include "../../Service/Inc/ExternalServerService.h"
#include "../../Service/Inc/CategoryService.h"
#include <functional>

class AdminController {
public:
    static void registerRoutes(HttpServer& server);

private:
    static void handleGetExternalServers(const httplib::Request& req, httplib::Response& res);
    static void handleGetExternalServerById(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateExternalServer(const httplib::Request& req, httplib::Response& res);
    static void handleAddCategory(const httplib::Request& req, httplib::Response& res);
    
    static ExternalServerService& getExternalServerService();
    static CategoryService& getCategoryService();
};
