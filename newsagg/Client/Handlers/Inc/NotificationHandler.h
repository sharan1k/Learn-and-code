#pragma once

#include "../../../Common/Inc/Dto/Notification.h"
#include "../../../Common/Inc/Dto/NotificationSetting.h"
#include "../../Http/Inc/HttpClient.h"
#include <functional>
#include <memory>
#include <vector>
#include <string>

using NotificationsCallback = std::function<void(bool success, const std::string& message, const std::vector<Notification*>& notifications)>;
using NotificationSettingsCallback = std::function<void(bool success, const std::string& message, const std::vector<NotificationSetting*>& settings)>;
using KeywordsCallback = std::function<void(bool success, const std::string& message, const std::vector<std::string>& keywords)>;
using SimpleCallback = std::function<void(bool success, const std::string& message)>;

class NotificationHandler {
public:
    NotificationHandler(std::shared_ptr<HttpClient> httpClient);
    void getNotifications(unsigned int userId, NotificationsCallback callback);
    void markNotificationsAsSeen(unsigned int userId, SimpleCallback callback);    
    void getNotificationSettings(unsigned int userId, NotificationSettingsCallback callback);    
    void updateNotificationSetting(unsigned int userId, unsigned int categoryId, bool enabled, SimpleCallback callback);    
    void getKeywords(unsigned int userId, KeywordsCallback callback);    
    void addKeyword(unsigned int userId, const std::string& keyword, SimpleCallback callback);    
    void removeKeyword(unsigned int userId, const std::string& keyword, SimpleCallback callback);

private:
    std::shared_ptr<HttpClient> httpClient;
};
