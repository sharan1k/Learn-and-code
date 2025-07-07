#include "../Inc/AdminController.h"

void AdminController::handleGetReportedArticles(const httplib::Request& req, httplib::Response& res) {
    try {
        auto& service = getAdminService();
        auto articles = service.getUnseenReportedArticles();
        
        nlohmann::json responseJson = nlohmann::json::array();
        for (const auto& article : articles) {
            responseJson.push_back(article->toJson());
        }
        
        sendSuccessResponse(res, responseJson);
    } catch (const std::exception& e) {
        sendErrorResponse(res, std::string("Error fetching reported articles: ") + e.what());
    }
}

void AdminController::handleHideArticle(const httplib::Request& req, httplib::Response& res) {
    try {
        if (!req.has_param("id")) {
            sendErrorResponse(res, "Article ID is required", 400);
            return;
        }
        
        unsigned int articleId = std::stoi(req.path_params.at("id"));
        auto& service = getAdminService();
        
        bool success = service.hideArticle(articleId);
        
        if (success) {
            sendSuccessResponse(res, nlohmann::json({}), 200, "Article has been hidden successfully");
        } else {
            sendErrorResponse(res, "Failed to hide article", 500);
        }
    } catch (const std::exception& e) {
        sendErrorResponse(res, std::string("Error hiding article: ") + e.what(), 500);
    }
}

void AdminController::handleUnhideArticle(const httplib::Request& req, httplib::Response& res) {
    try {
        if (!req.has_param("id")) {
            sendErrorResponse(res, "Article ID is required", 400);
            return;
        }
        
        unsigned int articleId = std::stoi(req.path_params.at("id"));
        auto& service = getAdminService();
        
        bool success = service.unhideArticle(articleId);
        
        if (success) {
            sendSuccessResponse(res, nlohmann::json({}), 200, "Article has been unhidden successfully");
        } else {
            sendErrorResponse(res, "Failed to unhide article", 500);
        }
    } catch (const std::exception& e) {
        sendErrorResponse(res, std::string("Error unhiding article: ") + e.what(), 500);
    }
}

void AdminController::handleHideCategory(const httplib::Request& req, httplib::Response& res) {
    try {
        if (!req.has_param("id")) {
            sendErrorResponse(res, "Category ID is required", 400);
            return;
        }
        
        unsigned int categoryId = std::stoi(req.path_params.at("id"));
        auto& service = getAdminService();
        
        bool success = service.hideCategory(categoryId);
        
        if (success) {
            sendSuccessResponse(res, nlohmann::json({}), 200, "Category has been hidden successfully");
        } else {
            sendErrorResponse(res, "Failed to hide category", 500);
        }
    } catch (const std::exception& e) {
        sendErrorResponse(res, std::string("Error hiding category: ") + e.what(), 500);
    }
}

void AdminController::handleUnhideCategory(const httplib::Request& req, httplib::Response& res) {
    try {
        if (!req.has_param("id")) {
            sendErrorResponse(res, "Category ID is required", 400);
            return;
        }
        
        unsigned int categoryId = std::stoi(req.path_params.at("id"));
        auto& service = getAdminService();
        
        bool success = service.unhideCategory(categoryId);
        
        if (success) {
            sendSuccessResponse(res, nlohmann::json({}), 200, "Category has been unhidden successfully");
        } else {
            sendErrorResponse(res, "Failed to unhide category", 500);
        }
    } catch (const std::exception& e) {
        sendErrorResponse(res, std::string("Error unhiding category: ") + e.what(), 500);
    }
}

void AdminController::handleGetHiddenArticles(const httplib::Request& req, httplib::Response& res) {
    try {
        auto& service = getAdminService();
        auto hiddenArticleIds = service.getAllHiddenArticleIds();
        
        nlohmann::json responseJson = nlohmann::json::array();
        for (const auto& id : hiddenArticleIds) {
            responseJson.push_back({{"articleId", id}});
        }
        
        sendSuccessResponse(res, responseJson);
    } catch (const std::exception& e) {
        sendErrorResponse(res, std::string("Error fetching hidden articles: ") + e.what());
    }
}

void AdminController::handleGetHiddenCategories(const httplib::Request& req, httplib::Response& res) {
    try {
        auto& service = getAdminService();
        auto hiddenCategoryIds = service.getAllHiddenCategoryIds();
        
        nlohmann::json responseJson = nlohmann::json::array();
        for (const auto& id : hiddenCategoryIds) {
            responseJson.push_back({{"categoryId", id}});
        }
        
        sendSuccessResponse(res, responseJson);
    } catch (const std::exception& e) {
        sendErrorResponse(res, std::string("Error fetching hidden categories: ") + e.what());
    }
}
