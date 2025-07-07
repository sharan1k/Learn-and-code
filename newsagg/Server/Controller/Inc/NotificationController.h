#pragma once

#include "../../Service/Inc/NotificationService.h"
#include "../../Http/Inc/HttpServer.h"
#include <string>
#include <nlohmann/json.hpp>

class NotificationController {
public:
    static void registerRoutes(HttpServer& server);

private:
    static void handleGetNotifications(const httplib::Request& request, httplib::Response& response);
    static void handleMarkNotificationsAsSeen(const httplib::Request& request, httplib::Response& response);
    static void handleGetNotificationSettings(const httplib::Request& request, httplib::Response& response);
    static void handleUpdateNotificationSetting(const httplib::Request& request, httplib::Response& response);
    static void handleGetKeywords(const httplib::Request& request, httplib::Response& response);
    static void handleAddKeyword(const httplib::Request& request, httplib::Response& response);
    static void handleRemoveKeyword(const httplib::Request& request, httplib::Response& response);
    
    static void sendSuccessResponse(httplib::Response& response, const nlohmann::json& data, int status = 200, const std::string& message = "");
    static void sendErrorResponse(httplib::Response& response, const std::string& message, int status = 500);
    
    static NotificationService& getNotificationService();
    static unsigned int getUserIdFromRequest(const httplib::Request& request);
    static bool validateUserId(unsigned int userId, httplib::Response& response);
};
