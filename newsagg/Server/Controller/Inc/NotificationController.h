#pragma once

#include "../../Service/Inc/NotificationService.h"
#include "../../Http/Inc/HttpServer.h"
#include <string>
#include <nlohmann/json.hpp>

class NotificationController {
public:
    static void registerRoutes(HttpServer& server);

private:
    static void handleGetNotifications(const httplib::Request& req, httplib::Response& res);
    static void handleMarkNotificationsAsSeen(const httplib::Request& req, httplib::Response& res);
    static void handleGetNotificationSettings(const httplib::Request& req, httplib::Response& res);
    static void handleUpdateNotificationSetting(const httplib::Request& req, httplib::Response& res);
    static void handleGetKeywords(const httplib::Request& req, httplib::Response& res);
    static void handleAddKeyword(const httplib::Request& req, httplib::Response& res);
    static void handleRemoveKeyword(const httplib::Request& req, httplib::Response& res);
    
    static void sendSuccessResponse(httplib::Response& res, const nlohmann::json& data, int status = 200, const std::string& message = "");
    static void sendErrorResponse(httplib::Response& res, const std::string& message, int status = 500);
    
    static NotificationService& getNotificationService();
    static unsigned int getUserIdFromRequest(const httplib::Request& req);
    static bool validateUserId(unsigned int userId, httplib::Response& res);
};
