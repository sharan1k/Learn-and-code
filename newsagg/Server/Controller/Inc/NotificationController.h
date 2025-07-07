#pragma once

#include "../../Service/Inc/NotificationService.h"
#include "../../Http/Inc/HttpServer.h"
#include <string>

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
    
    static NotificationService& getNotificationService();
    static unsigned int getUserIdFromRequest(const httplib::Request& req);
};
