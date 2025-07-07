#include "../Inc/ArticleController.h"
#include "../../Service/Inc/AdminService.h"
#include "../../Utils/Inc/Logger.h"

void ArticleController::handleReportArticle(const httplib::Request& request, httplib::Response& response) {
    try {
        if (request.path_params.find("articleId") == request.path_params.end()) {
            sendErrorResponse(response, "Article ID is required", 400);
            return;
        }
        
        if (!request.has_param("userId") && !request.body.empty()) {
            nlohmann::json reqJson = nlohmann::json::parse(request.body);
            if (!reqJson.contains("userId")) {
                sendErrorResponse(response, "User ID is required", 400);
                return;
            }
        }
        
        unsigned int articleId = std::stoi(request.path_params.at("articleId"));
        
        unsigned int userId;
        if (request.has_param("userId")) {
            userId = std::stoi(request.get_param_value("userId"));
        } else {
            nlohmann::json reqJson = nlohmann::json::parse(request.body);
            userId = reqJson["userId"].get<unsigned int>();
        }
        
        AdminService& adminService = AdminService::getInstance();
        bool success = adminService.reportArticle(userId, articleId);
        
        if (success) {
            Logger::info("User " + std::to_string(userId) + " reported article " + std::to_string(articleId));
            sendSuccessResponse(response, nlohmann::json({}), 200, "Article reported successfully");
        } else {
            sendErrorResponse(response, "Failed to report article", 500);
        }
    } catch (const std::exception& exception) {
        Logger::error("Error in handleReportArticle: " + std::string(exception.what()));
        sendErrorResponse(response, std::string("Error reporting article: ") + exception.what(), 500);
    }
}
