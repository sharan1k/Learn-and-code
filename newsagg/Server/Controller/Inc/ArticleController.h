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
    static void handleGetTodayHeadlines(const httplib::Request& request, httplib::Response& response);
    static void handleGetHeadlinesByDateRange(const httplib::Request& request, httplib::Response& response);
    static void handleGetHeadlinesByCategory(const httplib::Request& request, httplib::Response& response);
    static void handleGetArticleDetails(const httplib::Request& request, httplib::Response& response);
    static void handleSaveArticle(const httplib::Request& request, httplib::Response& response);
    static void handleGetSavedArticles(const httplib::Request& request, httplib::Response& response);
    static void handleRemoveSavedArticle(const httplib::Request& request, httplib::Response& response);
    static void handleSearchArticles(const httplib::Request& request, httplib::Response& response);
    static void handleLikeArticle(const httplib::Request& request, httplib::Response& response);
    static void handleDislikeArticle(const httplib::Request& request, httplib::Response& response);
    static void handleReportArticle(const httplib::Request& request, httplib::Response& response);
    static void handleGetCategories(const httplib::Request& request, httplib::Response& response);
    
    static void sendSuccessResponse(httplib::Response& response, const nlohmann::json& data, int status = 200, const std::string& message = "");
    static void sendErrorResponse(httplib::Response& response, const std::string& message, int status = 500);
    
    static ArticleService& getArticleService();
    static bool isValidDateFormat(const std::string& date);    
    static unsigned int getUserIdFromRequest(const httplib::Request& request);
    static unsigned int getArticleIdFromRequest(const httplib::Request& request);
    static int getLimitFromRequest(const httplib::Request& request, int defaultLimit = 10);
};
