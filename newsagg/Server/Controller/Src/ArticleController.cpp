#include "../Inc/ArticleController.h"
#include <regex>

ArticleService& ArticleController::getArticleService() {
    static ArticleService service;
    return service;
}

void ArticleController::sendSuccessResponse(httplib::Response& res, const nlohmann::json& data, int status, const std::string& message) {
    nlohmann::json response = {{"status", "success"}};
    
    if (!data.is_null()) {
        response["data"] = data;
    }
    
    if (!message.empty()) {
        response["message"] = message;
    }
    
    res.status = status;
    res.set_content(response.dump(), "application/json");
}

void ArticleController::sendErrorResponse(httplib::Response& res, const std::string& message, int status) {
    nlohmann::json response = {
        {"status", "error"},
        {"message", message}
    };
    
    res.status = status;
    res.set_content(response.dump(), "application/json");
}

void ArticleController::registerRoutes(HttpServer& server) {
    server.get("/api/search", handleSearchArticles);
    server.get("/api/articles/headlines/today", handleGetTodayHeadlines);
    server.get("/api/articles/headlines/date-range", handleGetHeadlinesByDateRange);
    server.get("/api/articles/headlines/category/:categoryId", handleGetHeadlinesByCategory);
    server.get("/api/articles/:articleId", handleGetArticleDetails);
    server.post("/api/users/:userId/saved-articles", handleSaveArticle);
    server.get("/api/users/:userId/saved-articles", handleGetSavedArticles);
    server.del("/api/users/:userId/saved-articles/:articleId", handleRemoveSavedArticle);
    server.post("/api/articles/:articleId/like", handleLikeArticle);
    server.post("/api/articles/:articleId/dislike", handleDislikeArticle);
    server.get("/api/categories", handleGetCategories);
}

bool ArticleController::isValidDateFormat(const std::string& date) {
    std::regex datePattern(R"(\d{4}-\d{2}-\d{2})");
    return std::regex_match(date, datePattern);
}

unsigned int ArticleController::getUserIdFromRequest(const httplib::Request& req) {
    if (req.path_params.find("userId") != req.path_params.end()) {
        try {
            return std::stoul(req.path_params.at("userId"));
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

unsigned int ArticleController::getArticleIdFromRequest(const httplib::Request& req) {
    if (req.path_params.find("articleId") != req.path_params.end()) {
        try {
            return std::stoul(req.path_params.at("articleId"));
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

int ArticleController::getLimitFromRequest(const httplib::Request& req, int defaultLimit) {
    if (req.has_param("limit")) {
        try {
            return std::stoi(req.get_param_value("limit"));
        } catch (...) {
            return defaultLimit;
        }
    }
    return defaultLimit;
}

void ArticleController::handleGetTodayHeadlines(const httplib::Request& req, httplib::Response& res) {
    try {
        int limit = getLimitFromRequest(req);
        auto articles = getArticleService().getTodayHeadlines(limit);
        
        nlohmann::json responseData = nlohmann::json::array();
        for (const auto& article : articles) {
            responseData.push_back(article->toJson());
        }
        
        sendSuccessResponse(res, responseData, 200, "Headlines retrieved successfully");
    } catch (const std::exception& e) {
        sendErrorResponse(res, "Server error: " + std::string(e.what()));
    }
}

void ArticleController::handleGetHeadlinesByDateRange(const httplib::Request& req, httplib::Response& res) {
    try {
        if (!req.has_param("startDate") || !req.has_param("endDate")) {
            sendErrorResponse(res, "Start date and end date are required", 400);
            return;
        }
        
        std::string startDate = req.get_param_value("startDate");
        std::string endDate = req.get_param_value("endDate");
        
        if (!isValidDateFormat(startDate) || !isValidDateFormat(endDate)) {
            sendErrorResponse(res, "Invalid date format. Use YYYY-MM-DD format.", 400);
            return;
        }
        
        int limit = getLimitFromRequest(req);
        std::vector<std::shared_ptr<Article>> articles;
        
        if (req.has_param("categoryId")) {
            unsigned int categoryId = std::stoul(req.get_param_value("categoryId"));
            articles = getArticleService().getHeadlinesByDateRangeAndCategory(startDate, endDate, categoryId, limit);
        } else {
            articles = getArticleService().getHeadlinesByDateRange(startDate, endDate, limit);
        }
        
        nlohmann::json responseData = nlohmann::json::array();
        for (const auto& article : articles) {
            responseData.push_back(article->toJson());
        }
        
        sendSuccessResponse(res, responseData, 200, "Headlines retrieved successfully");
    } catch (const std::exception& e) {
        sendErrorResponse(res, "Server error: " + std::string(e.what()));
    }
}

void ArticleController::handleGetHeadlinesByCategory(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling get headlines by category request" << std::endl;
    
    try {
        if (req.path_params.find("categoryId") == req.path_params.end()) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Category ID is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int categoryId;
        try {
            categoryId = std::stoul(req.path_params.at("categoryId"));
        } catch (...) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid category ID"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        int limit = 10;
        if (req.has_param("limit")) {
            limit = std::stoi(req.get_param_value("limit"));
        }
        
        if (req.has_param("startDate") && req.has_param("endDate")) {
            std::string startDate = req.get_param_value("startDate");
            std::string endDate = req.get_param_value("endDate");
            
            if (isValidDateFormat(startDate) && isValidDateFormat(endDate)) {
                auto articles = getArticleService().getHeadlinesByDateRangeAndCategory(startDate, endDate, categoryId, limit);
                
                nlohmann::json jsonResponse = {
                    {"status", "success"},
                    {"message", "Headlines retrieved successfully"},
                    {"data", nlohmann::json::array()}
                };
                
                for (const auto& article : articles) {
                    jsonResponse["data"].push_back(article->toJson());
                }
                
                res.set_content(jsonResponse.dump(), "application/json");
                return;
            }
        }
        
        auto articles = getArticleService().getTodayHeadlinesByCategory(categoryId, limit);
        
        nlohmann::json jsonResponse = {
            {"status", "success"},
            {"message", "Headlines retrieved successfully"},
            {"data", nlohmann::json::array()}
        };
        
        for (const auto& article : articles) {
            jsonResponse["data"].push_back(article->toJson());
        }
        
        res.set_content(jsonResponse.dump(), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void ArticleController::handleGetArticleDetails(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling get article details request" << std::endl;
    
    try {
        if (req.path_params.find("articleId") == req.path_params.end()) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Article ID is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int articleId;
        try {
            articleId = std::stoul(req.path_params.at("articleId"));
        } catch (...) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid article ID"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        auto article = getArticleService().getArticleDetails(articleId);
        
        if (!article) {
            res.status = 404;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Article not found"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        nlohmann::json jsonResponse = {
            {"status", "success"},
            {"message", "Article details retrieved successfully"},
            {"data", article->toJson()}
        };
        
        res.set_content(jsonResponse.dump(), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void ArticleController::handleSaveArticle(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling save article request" << std::endl;
    
    try {
        unsigned int userId = getUserIdFromRequest(req);
        if (userId == 0) {
            res.status = 401;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Unauthorized"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        nlohmann::json requestData = nlohmann::json::parse(req.body);
        
        if (!requestData.contains("articleId")) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Article ID is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int articleId = requestData["articleId"].get<unsigned int>();
        
        bool success = getArticleService().saveArticle(userId, articleId);
        
        if (success) {
            nlohmann::json jsonResponse = {
                {"status", "success"},
                {"message", "Article saved successfully"}
            };
            res.set_content(jsonResponse.dump(), "application/json");
        } else {
            res.status = 404;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to save article. Article might not exist."}
            };
            res.set_content(errorResponse.dump(), "application/json");
        }
        
    } catch (const nlohmann::json::parse_error& e) {
        res.status = 400;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Invalid JSON format: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void ArticleController::handleGetSavedArticles(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling get saved articles request" << std::endl;
    
    try {
        unsigned int userId = getUserIdFromRequest(req);
        if (userId == 0) {
            res.status = 401;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Unauthorized"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        auto articles = getArticleService().getSavedArticles(userId);
        
        nlohmann::json jsonResponse = {
            {"status", "success"},
            {"message", "Saved articles retrieved successfully"},
            {"data", nlohmann::json::array()}
        };
        
        for (const auto& article : articles) {
            jsonResponse["data"].push_back(article->toJson());
        }
        
        res.set_content(jsonResponse.dump(), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void ArticleController::handleRemoveSavedArticle(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling remove saved article request" << std::endl;
    
    try {
        unsigned int userId = getUserIdFromRequest(req);
        if (userId == 0) {
            res.status = 401;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Unauthorized"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        if (req.path_params.find("articleId") == req.path_params.end()) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Article ID is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int articleId;
        try {
            articleId = std::stoul(req.path_params.at("articleId"));
        } catch (...) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid article ID"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        bool success = getArticleService().removeSavedArticle(userId, articleId);
        
        if (success) {
            nlohmann::json jsonResponse = {
                {"status", "success"},
                {"message", "Saved article removed successfully"}
            };
            res.set_content(jsonResponse.dump(), "application/json");
        } else {
            res.status = 404;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to remove saved article. It might not exist."}
            };
            res.set_content(errorResponse.dump(), "application/json");
        }
        
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void ArticleController::handleSearchArticles(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling search articles request" << std::endl;
    
    try {
        if (!req.has_param("query")) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Search query is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        std::string query = req.get_param_value("query");
        
        int limit = 10; 
        if (req.has_param("limit")) {
            limit = std::stoi(req.get_param_value("limit"));
        }
        
        bool hasDateRange = req.has_param("startDate") && req.has_param("endDate");
        std::string startDate = hasDateRange ? req.get_param_value("startDate") : "";
        std::string endDate = hasDateRange ? req.get_param_value("endDate") : "";
        
        if (hasDateRange && (!isValidDateFormat(startDate) || !isValidDateFormat(endDate))) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid date format. Use YYYY-MM-DD format."}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        bool hasSortParam = req.has_param("sortBy");
        std::string sortBy = hasSortParam ? req.get_param_value("sortBy") : "date";
        
        bool sortByLikes = sortBy == "likes";
        bool sortByDislikes = sortBy == "dislikes";
        bool isDescending = true; 
        
        if (req.has_param("order")) {
            isDescending = req.get_param_value("order") == "desc";
        }
        
        std::vector<std::shared_ptr<Article>> articles;
        
        if (hasDateRange) {
            if (sortByLikes || sortByDislikes) {
                articles = getArticleService().searchArticlesByDateRangeSortedByLikes(
                    query, startDate, endDate, sortByLikes, isDescending, limit);
            } else {
                articles = getArticleService().searchArticlesByDateRange(
                    query, startDate, endDate, limit);
            }
        } else {
            if (sortByLikes || sortByDislikes) {
                articles = getArticleService().searchArticlesSortedByLikes(
                    query, sortByLikes, isDescending, limit);
            } else {
                articles = getArticleService().searchArticles(query, limit);
            }
        }
        
        nlohmann::json jsonResponse = {
            {"status", "success"},
            {"message", "Search results retrieved successfully"},
            {"data", nlohmann::json::array()}
        };
        
        for (const auto& article : articles) {
            jsonResponse["data"].push_back(article->toJson());
        }
        
        res.set_content(jsonResponse.dump(), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void ArticleController::handleGetCategories(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling get categories request" << std::endl;
    
    try {
        auto categories = getArticleService().getAllCategories();
        
        nlohmann::json jsonResponse = {
            {"status", "success"},
            {"message", "Categories retrieved successfully"},
            {"data", nlohmann::json::array()}
        };
        
        for (const auto& category : categories) {
            jsonResponse["data"].push_back(category->toJson());
        }
        
        res.set_content(jsonResponse.dump(), "application/json");
        
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void ArticleController::handleLikeArticle(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling like article request" << std::endl;
    
    try {
        if (req.path_params.find("articleId") == req.path_params.end()) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Article ID is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int articleId;
        try {
            articleId = std::stoul(req.path_params.at("articleId"));
        } catch (...) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid article ID format"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        nlohmann::json requestData = nlohmann::json::parse(req.body);
        
        if (!requestData.contains("userId")) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "User ID is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int userId = requestData["userId"].get<unsigned int>();
        
        bool success = getArticleService().likeArticle(userId, articleId);
        
        if (success) {
            nlohmann::json jsonResponse = {
                {"status", "success"},
                {"message", "Article liked successfully"}
            };
            res.set_content(jsonResponse.dump(), "application/json");
        } else {
            res.status = 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to like article"}
            };
            res.set_content(errorResponse.dump(), "application/json");
        }
        
    } catch (const nlohmann::json::parse_error& e) {
        res.status = 400;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Invalid JSON format: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void ArticleController::handleDislikeArticle(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling dislike article request" << std::endl;
    
    try {
        if (req.path_params.find("articleId") == req.path_params.end()) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Article ID is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int articleId;
        try {
            articleId = std::stoul(req.path_params.at("articleId"));
        } catch (...) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid article ID format"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        nlohmann::json requestData = nlohmann::json::parse(req.body);
        
        if (!requestData.contains("userId")) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "User ID is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int userId = requestData["userId"].get<unsigned int>();
        
        bool success = getArticleService().dislikeArticle(userId, articleId);
        
        if (success) {
            nlohmann::json jsonResponse = {
                {"status", "success"},
                {"message", "Article disliked successfully"}
            };
            res.set_content(jsonResponse.dump(), "application/json");
        } else {
            res.status = 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to dislike article"}
            };
            res.set_content(errorResponse.dump(), "application/json");
        }
        
    } catch (const nlohmann::json::parse_error& e) {
        res.status = 400;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Invalid JSON format: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}




