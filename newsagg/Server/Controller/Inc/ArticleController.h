#pragma once

#include "../../../Common/Inc/Dto/Article.h"
#include "../../Service/Inc/ArticleService.h"
#include "../../Http/Inc/HttpServer.h"
#include <functional>
#include <string>
#include <nlohmann/json.hpp>

class ArticleController {
public:
    static void registerRoutes(HttpServer& server);

private:
    static void handleGetTodayHeadlines(const httplib::Request& req, httplib::Response& res);
    static void handleGetHeadlinesByDateRange(const httplib::Request& req, httplib::Response& res);
    static void handleGetHeadlinesByCategory(const httplib::Request& req, httplib::Response& res);
    static void handleGetArticleDetails(const httplib::Request& req, httplib::Response& res);
    static void handleSaveArticle(const httplib::Request& req, httplib::Response& res);
    static void handleGetSavedArticles(const httplib::Request& req, httplib::Response& res);
    static void handleRemoveSavedArticle(const httplib::Request& req, httplib::Response& res);
    static void handleSearchArticles(const httplib::Request& req, httplib::Response& res);
    static void handleLikeArticle(const httplib::Request& req, httplib::Response& res);
    static void handleDislikeArticle(const httplib::Request& req, httplib::Response& res);
    static void handleReportArticle(const httplib::Request& req, httplib::Response& res);
    static void handleGetCategories(const httplib::Request& req, httplib::Response& res);
    
    static void sendSuccessResponse(httplib::Response& res, const nlohmann::json& data, int status = 200, const std::string& message = "");
    static void sendErrorResponse(httplib::Response& res, const std::string& message, int status = 500);
    
    static ArticleService& getArticleService();
    static bool isValidDateFormat(const std::string& date);    
    static unsigned int getUserIdFromRequest(const httplib::Request& req);
    static unsigned int getArticleIdFromRequest(const httplib::Request& req);
    static int getLimitFromRequest(const httplib::Request& req, int defaultLimit = 10);
};
