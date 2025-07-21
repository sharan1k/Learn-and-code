#include "../Inc/AdminController.h"

void AdminController::handleGetReportedArticles(const httplib::Request& request, httplib::Response& response) {
    try {
        auto& service = getAdminService();
        auto articles = service.getUnseenReportedArticles();
        
        nlohmann::json responseJson = nlohmann::json::array();
        for (const auto& article : articles) {
            responseJson.push_back(article->toJson());
        }
        
        sendSuccessResponse(response, responseJson);
    } catch (const std::exception& exception) {
        sendErrorResponse(response, std::string("Error fetching reported articles: ") + exception.what());
    }
}

void AdminController::handleHideArticle(const httplib::Request& request, httplib::Response& response) {
    try {
        if (!request.has_param("id")) {
            sendErrorResponse(response, "Article ID is required", 400);
            return;
        }
        
        unsigned int articleId = std::stoi(request.path_params.at("id"));
        auto& service = getAdminService();
        
        bool success = service.hideArticle(articleId);
        
        if (success) {
            sendSuccessResponse(response, nlohmann::json({}), 200, "Article has been hidden successfully");
        } else {
            sendErrorResponse(response, "Failed to hide article", 500);
        }
    } catch (const std::exception& exception) {
        sendErrorResponse(response, std::string("Error hiding article: ") + exception.what(), 500);
    }
}

void AdminController::handleUnhideArticle(const httplib::Request& request, httplib::Response& response) {
    try {
        if (!request.has_param("id")) {
            sendErrorResponse(response, "Article ID is required", 400);
            return;
        }
        
        unsigned int articleId = std::stoi(request.path_params.at("id"));
        auto& service = getAdminService();
        
        bool success = service.unhideArticle(articleId);
        
        if (success) {
            sendSuccessResponse(response, nlohmann::json({}), 200, "Article has been unhidden successfully");
        } else {
            sendErrorResponse(response, "Failed to unhide article", 500);
        }
    } catch (const std::exception& exception) {
        sendErrorResponse(response, std::string("Error unhiding article: ") + exception.what(), 500);
    }
}

void AdminController::handleHideCategory(const httplib::Request& request, httplib::Response& response) {
    try {
        if (!request.has_param("id")) {
            sendErrorResponse(response, "Category ID is required", 400);
            return;
        }
        
        unsigned int categoryId = std::stoi(request.path_params.at("id"));
        auto& service = getAdminService();
        
        bool success = service.hideCategory(categoryId);
        
        if (success) {
            sendSuccessResponse(response, nlohmann::json({}), 200, "Category has been hidden successfully");
        } else {
            sendErrorResponse(response, "Failed to hide category", 500);
        }
    } catch (const std::exception& exception) {
        sendErrorResponse(response, std::string("Error hiding category: ") + exception.what(), 500);
    }
}

void AdminController::handleUnhideCategory(const httplib::Request& request, httplib::Response& response) {
    try {
        if (!request.has_param("id")) {
            sendErrorResponse(response, "Category ID is required", 400);
            return;
        }
        
        unsigned int categoryId = std::stoi(request.path_params.at("id"));
        auto& service = getAdminService();
        
        bool success = service.unhideCategory(categoryId);
        
        if (success) {
            sendSuccessResponse(response, nlohmann::json({}), 200, "Category has been unhidden successfully");
        } else {
            sendErrorResponse(response, "Failed to unhide category", 500);
        }
    } catch (const std::exception& exception) {
        sendErrorResponse(response, std::string("Error unhiding category: ") + exception.what(), 500);
    }
}

void AdminController::handleGetHiddenArticles(const httplib::Request& request, httplib::Response& response) {
    try {
        auto& service = getAdminService();
        auto hiddenArticleIds = service.getAllHiddenArticleIds();
        
        nlohmann::json responseJson = nlohmann::json::array();
        for (const auto& id : hiddenArticleIds) {
            responseJson.push_back({{"articleId", id}});
        }
        
        sendSuccessResponse(response, responseJson);
    } catch (const std::exception& exception) {
        sendErrorResponse(response, std::string("Error fetching hidden articles: ") + exception.what());
    }
}

void AdminController::handleGetHiddenCategories(const httplib::Request& request, httplib::Response& response) {
    try {
        auto& service = getAdminService();
        auto hiddenCategoryIds = service.getAllHiddenCategoryIds();
        
        nlohmann::json responseJson = nlohmann::json::array();
        for (const auto& id : hiddenCategoryIds) {
            responseJson.push_back({{"categoryId", id}});
        }
        
        sendSuccessResponse(response, responseJson);
    } catch (const std::exception& exception) {
        sendErrorResponse(response, std::string("Error fetching hidden categories: ") + exception.what());
    }
}
