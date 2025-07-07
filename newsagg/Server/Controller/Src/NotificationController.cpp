#include "../Inc/NotificationController.h"

NotificationService& NotificationController::getNotificationService() {
    static NotificationService service;
    return service;
}

void NotificationController::sendSuccessResponse(httplib::Response& response, const nlohmann::json& data, int status, const std::string& message) {
    nlohmann::json response = {{"status", "success"}};
    
    if (!data.is_null()) {
        response["data"] = data;
    }
    
    if (!message.empty()) {
        response["message"] = message;
    }
    
    response.status = status;
    response.set_content(response.dump(), "application/json");
}

void NotificationController::sendErrorResponse(httplib::Response& response, const std::string& message, int status) {
    nlohmann::json response = {
        {"status", "error"},
        {"message", message}
    };
    
    response.status = status;
    response.set_content(response.dump(), "application/json");
}

void NotificationController::registerRoutes(HttpServer& server) {
    server.get("/api/users/:userId/notifications", handleGetNotifications);
    server.put("/api/users/:userId/notifications/mark-seen", handleMarkNotificationsAsSeen);
    server.get("/api/users/:userId/notification-settings", handleGetNotificationSettings);
    server.post("/api/users/:userId/notification-settings", handleUpdateNotificationSetting);
    server.get("/api/users/:userId/keywords", handleGetKeywords);
    server.post("/api/users/:userId/keywords", handleAddKeyword);
    server.del("/api/users/:userId/keywords/:keyword", handleRemoveKeyword);
}

unsigned int NotificationController::getUserIdFromRequest(const httplib::Request& request) {
    if (request.path_params.find("userId") != request.path_params.end()) {
        try {
            return std::stoul(request.path_params.at("userId"));
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

bool NotificationController::validateUserId(unsigned int userId, httplib::Response& response) {
    if (userId == 0) {
        sendErrorResponse(response, "Invalid or missing user ID", 400);
        return false;
    }
    return true;
}

void NotificationController::handleGetNotifications(const httplib::Request& request, httplib::Response& response) {
    try {
        unsigned int userId = getUserIdFromRequest(request);
        
        if (!validateUserId(userId, response)) {
            return;
        }
        
        auto notifications = getNotificationService().getNotifications(userId);
        
        nlohmann::json responseData = nlohmann::json::array();
        for (const auto& notification : notifications) {
            responseData.push_back(notification->toJson());
        }
        
        sendSuccessResponse(response, responseData, 200, "Notifications retrieved successfully");
    } catch (const std::exception& exception) {
        sendErrorResponse(response, "Server error: " + std::string(exception.what()));
    }
}

void NotificationController::handleMarkNotificationsAsSeen(const httplib::Request& request, httplib::Response& response) {
    std::cout << "Handling mark notifications as seen request" << std::endl;
    
    try {
        unsigned int userId = getUserIdFromRequest(request);
        
        if (userId == 0) {
            response.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid or missing user ID"}
            };
            response.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        bool success = getNotificationService().markNotificationsAsSeen(userId);
        
        if (success) {
            nlohmann::json jsonResponse = {
                {"status", "success"},
                {"message", "Notifications marked as seen successfully"}
            };
            response.set_content(jsonResponse.dump(), "application/json");
        } else {
            response.status = 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to mark notifications as seen"}
            };
            response.set_content(errorResponse.dump(), "application/json");
        }
        
    } catch (const std::exception& exception) {
        response.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(exception.what())}
        };
        response.set_content(errorResponse.dump(), "application/json");
    }
}

void NotificationController::handleGetNotificationSettings(const httplib::Request& request, httplib::Response& response) {
    std::cout << "Handling get notification settings request" << std::endl;
    
    try {
        unsigned int userId = getUserIdFromRequest(request);
        
        if (userId == 0) {
            response.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid or missing user ID"}
            };
            response.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        auto settings = getNotificationService().getNotificationSettings(userId);
        
        nlohmann::json jsonResponse = {
            {"status", "success"},
            {"message", "Notification settings retrieved successfully"},
            {"data", nlohmann::json::array()}
        };
        
        for (const auto& setting : settings) {
            jsonResponse["data"].push_back(setting->toJson());
        }
        
        response.set_content(jsonResponse.dump(), "application/json");
        
    } catch (const std::exception& exception) {
        response.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(exception.what())}
        };
        response.set_content(errorResponse.dump(), "application/json");
    }
}

void NotificationController::handleUpdateNotificationSetting(const httplib::Request& request, httplib::Response& response) {
    std::cout << "Handling update notification setting request" << std::endl;
    
    try {
        unsigned int userId = getUserIdFromRequest(request);
        
        if (userId == 0) {
            response.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid or missing user ID"}
            };
            response.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        nlohmann::json requestData = nlohmann::json::parse(request.body);
        
        if (!requestData.contains("categoryId") || !requestData.contains("enabled")) {
            response.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Category ID and enabled flag are required"}
            };
            response.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int categoryId = requestData["categoryId"].get<unsigned int>();
        bool enabled = requestData["enabled"].get<bool>();
        
        bool success = getNotificationService().updateNotificationSetting(userId, categoryId, enabled);
        
        if (success) {
            nlohmann::json jsonResponse = {
                {"status", "success"},
                {"message", "Notification setting updated successfully"}
            };
            response.set_content(jsonResponse.dump(), "application/json");
        } else {
            response.status = 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to update notification setting"}
            };
            response.set_content(errorResponse.dump(), "application/json");
        }
        
    } catch (const nlohmann::json::parse_error& exception) {
        response.status = 400;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Invalid JSON format: " + std::string(exception.what())}
        };
        response.set_content(errorResponse.dump(), "application/json");
    } catch (const std::exception& exception) {
        response.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(exception.what())}
        };
        response.set_content(errorResponse.dump(), "application/json");
    }
}

void NotificationController::handleGetKeywords(const httplib::Request& request, httplib::Response& response) {
    std::cout << "Handling get keywords request" << std::endl;
    
    try {
        unsigned int userId = getUserIdFromRequest(request);
        
        if (userId == 0) {
            response.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid or missing user ID"}
            };
            response.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        auto keywords = getNotificationService().getKeywords(userId);
        
        nlohmann::json jsonResponse = {
            {"status", "success"},
            {"message", "Keywords retrieved successfully"},
            {"data", nlohmann::json::array()}
        };
        
        for (const auto& keyword : keywords) {
            jsonResponse["data"].push_back({{"keyword", keyword}});
        }
        
        response.set_content(jsonResponse.dump(), "application/json");
        
    } catch (const std::exception& exception) {
        response.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(exception.what())}
        };
        response.set_content(errorResponse.dump(), "application/json");
    }
}

void NotificationController::handleAddKeyword(const httplib::Request& request, httplib::Response& response) {
    std::cout << "Handling add keyword request" << std::endl;
    
    try {
        unsigned int userId = getUserIdFromRequest(request);
        
        if (userId == 0) {
            response.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid or missing user ID"}
            };
            response.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        nlohmann::json requestData = nlohmann::json::parse(request.body);
        
        if (!requestData.contains("keyword") || requestData["keyword"].get<std::string>().empty()) {
            response.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Keyword is required and cannot be empty"}
            };
            response.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        std::string keyword = requestData["keyword"].get<std::string>();
        
        bool success = getNotificationService().addKeyword(userId, keyword);
        
        if (success) {
            nlohmann::json jsonResponse = {
                {"status", "success"},
                {"message", "Keyword added successfully"}
            };
            response.set_content(jsonResponse.dump(), "application/json");
        } else {
            response.status = 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to add keyword"}
            };
            response.set_content(errorResponse.dump(), "application/json");
        }
        
    } catch (const nlohmann::json::parse_error& exception) {
        response.status = 400;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Invalid JSON format: " + std::string(exception.what())}
        };
        response.set_content(errorResponse.dump(), "application/json");
    } catch (const std::exception& exception) {
        response.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(exception.what())}
        };
        response.set_content(errorResponse.dump(), "application/json");
    }
}

void NotificationController::handleRemoveKeyword(const httplib::Request& request, httplib::Response& response) {
    std::cout << "Handling remove keyword request" << std::endl;
    
    try {
        unsigned int userId = getUserIdFromRequest(request);
        
        if (userId == 0) {
            response.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid or missing user ID"}
            };
            response.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        if (request.path_params.find("keyword") == request.path_params.end()) {
            response.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Keyword parameter is required"}
            };
            response.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        std::string keyword = request.path_params.at("keyword");
        
        bool success = getNotificationService().removeKeyword(userId, keyword);
        
        if (success) {
            nlohmann::json jsonResponse = {
                {"status", "success"},
                {"message", "Keyword removed successfully"}
            };
            response.set_content(jsonResponse.dump(), "application/json");
        } else {
            response.status = 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to remove keyword"}
            };
            response.set_content(errorResponse.dump(), "application/json");
        }
        
    } catch (const std::exception& exception) {
        response.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(exception.what())}
        };
        response.set_content(errorResponse.dump(), "application/json");
    }
}
