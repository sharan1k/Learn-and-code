#ifndef NOTIFICATION_HANDLER_H
#define NOTIFICATION_HANDLER_H

#include "../../../Common/Inc/Dto/Notification.h"
#include "../../../Common/Inc/Dto/NotificationSetting.h"
#include "../../Http/Inc/HttpClient.h"
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <map>

class NotificationHandler {
public:
    using NotificationsCallback = std::function<void(bool success, const std::string& message, const std::vector<Notification*>& notifications)>;
    using SettingsCallback = std::function<void(bool success, const std::string& message, const std::vector<NotificationSetting*>& settings)>;
    using KeywordsCallback = std::function<void(bool success, const std::string& message, const std::vector<std::string>& keywords)>;
    using StatusCallback = std::function<void(bool success, const std::string& message)>;
    
    explicit NotificationHandler(std::shared_ptr<HttpClient> httpClient);
    
    void getNotifications(unsigned int userId, NotificationsCallback callback);
    void markNotificationsAsSeen(unsigned int userId, StatusCallback callback);
    
    void getNotificationSettings(unsigned int userId, SettingsCallback callback);
    void updateNotificationSetting(unsigned int userId, unsigned int categoryId, bool enabled, StatusCallback callback);
    
    void getKeywords(unsigned int userId, KeywordsCallback callback);
    void addKeyword(unsigned int userId, const std::string& keyword, StatusCallback callback);
    void removeKeyword(unsigned int userId, const std::string& keyword, StatusCallback callback);

private:
    std::shared_ptr<HttpClient> httpClient;
    
    std::string extractErrorMessage(
        const httplib::Result& result,
        const std::string& defaultMessage,
        const std::map<int, std::string>& statusCodes = {});
    
    nlohmann::json parseResponse(
        const httplib::Result& result,
        bool& success,
        std::string& errorMessage);
};

#endif // NOTIFICATION_HANDLER_H
