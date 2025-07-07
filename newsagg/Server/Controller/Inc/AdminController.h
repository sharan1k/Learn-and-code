#pragma once

#include "../../../Common/Inc/Dto/ExternalServer.h"
#include "../../../Common/Inc/Dto/Category.h"
#include "../../../Common/Inc/Dto/Article.h"
#include "../../../Common/Inc/Dto/ReportedArticle.h"
#include "../../Http/Inc/HttpServer.h"
#include "../../Service/Inc/ExternalServerService.h"
#include "../../Service/Inc/CategoryService.h"
#include "../../Service/Inc/AdminService.h"
#include <functional>
#include <nlohmann/json.hpp>

class AdminController {
public:
    static void registerRoutes(HttpServer& server);

private:
    static void handleGetExternalServers(const httplib::Request& req, httplib::Response& res);
    static void handleGetExternalServerById(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateExternalServer(const httplib::Request& req, httplib::Response& res);
    static void handleAddCategory(const httplib::Request& req, httplib::Response& res);
    static void handleGetReportedArticles(const httplib::Request& req, httplib::Response& res);
    static void handleHideArticle(const httplib::Request& req, httplib::Response& res);
    static void handleUnhideArticle(const httplib::Request& req, httplib::Response& res);
    static void handleHideCategory(const httplib::Request& req, httplib::Response& res);
    static void handleUnhideCategory(const httplib::Request& req, httplib::Response& res);
    static void handleGetHiddenArticles(const httplib::Request& req, httplib::Response& res);
    static void handleGetHiddenCategories(const httplib::Request& req, httplib::Response& res);
    
    static ExternalServerService& getExternalServerService();
    static CategoryService& getCategoryService();
    static AdminService& getAdminService();
    
    static void sendSuccessResponse(httplib::Response& res, const nlohmann::json& data, int status = 200, const std::string& message = "");
    static void sendErrorResponse(httplib::Response& res, const std::string& message, int status = 500);
};
