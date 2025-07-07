#include "../Inc/ArticleController.h"
#include "../../Service/Inc/AdminService.h"
#include "../../Utils/Inc/Logger.h"

void ArticleController::handleReportArticle(const httplib::Request& req, httplib::Response& res) {
    try {
        if (req.path_params.find("articleId") == req.path_params.end()) {
            sendErrorResponse(res, "Article ID is required", 400);
            return;
        }
        
        if (!req.has_param("userId") && !req.body.empty()) {
            nlohmann::json reqJson = nlohmann::json::parse(req.body);
            if (!reqJson.contains("userId")) {
                sendErrorResponse(res, "User ID is required", 400);
                return;
            }
        }
        
        unsigned int articleId = std::stoi(req.path_params.at("articleId"));
        
        unsigned int userId;
        if (req.has_param("userId")) {
            userId = std::stoi(req.get_param_value("userId"));
        } else {
            nlohmann::json reqJson = nlohmann::json::parse(req.body);
            userId = reqJson["userId"].get<unsigned int>();
        }
        
        AdminService& adminService = AdminService::getInstance();
        bool success = adminService.reportArticle(userId, articleId);
        
        if (success) {
            Logger::info("User " + std::to_string(userId) + " reported article " + std::to_string(articleId));
            sendSuccessResponse(res, nlohmann::json({}), 200, "Article reported successfully");
        } else {
            sendErrorResponse(res, "Failed to report article", 500);
        }
    } catch (const std::exception& e) {
        Logger::error("Error in handleReportArticle: " + std::string(e.what()));
        sendErrorResponse(res, std::string("Error reporting article: ") + e.what(), 500);
    }
}
