#include "../Inc/NotificationHandler.h"
#include <nlohmann/json.hpp>
#include <iostream>

NotificationHandler::NotificationHandler(std::shared_ptr<HttpClient> httpClient)
    : httpClient(httpClient) {
}

std::string NotificationHandler::extractErrorMessage(
    const httplib::Result& result, 
    const std::string& defaultMessage,
    const std::map<int, std::string>& statusCodes) {
    
    std::string errorMessage = defaultMessage;
    
    try {
        auto errorJson = nlohmann::json::parse(result->body);
        
        if (errorJson.contains("message") && errorJson["message"].is_string()) {
            errorMessage = errorJson["message"];
        }
        
        auto it = statusCodes.find(result->status);
        if (it != statusCodes.end()) {
            errorMessage = it->second;
        }
    } catch (...) {
        errorMessage = defaultMessage + " (status " + std::to_string(result->status) + ")";
    }
    
    return errorMessage;
}

nlohmann::json NotificationHandler::parseResponse(
    const httplib::Result& result, 
    bool& success, 
    std::string& errorMessage) {
    
    if (!result) {
        success = false;
        errorMessage = "Network error";
        return nlohmann::json();
    }
    
    try {
        nlohmann::json response = nlohmann::json::parse(result->body);
        if (response["status"] == "success") {
            success = true;
            return response;
        } else {
            success = false;
            errorMessage = response.value("message", "Unknown error");
            return nlohmann::json();
        }
    } catch (const std::exception& exception) {
        success = false;
        errorMessage = "Error parsing server response: " + std::string(exception.what());
        return nlohmann::json();
    }
}

void NotificationHandler::getNotifications(unsigned int userId, NotificationsCallback callback) {
    const std::string endpoint = "/api/users/" + std::to_string(userId) + "/notifications";
    
    httpClient->get(endpoint, [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to fetch notifications"
            );
            callback(false, errorMessage, {});
            return;
        }
        
        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage, {});
            return;
        }
        
        try {
            std::vector<Notification*> notifications;
            
            for (const auto& notificationJson : response["data"]) {
                Notification* notification = new Notification(Notification::fromJson(notificationJson));
                notifications.push_back(notification);
            }
            
            callback(true, response.value("message", "Notifications retrieved successfully"), notifications);
        } catch (const std::exception& exception) {
            callback(false, "Error processing notifications data: " + std::string(exception.what()), {});
        }
    });
}

void NotificationHandler::markNotificationsAsSeen(unsigned int userId, StatusCallback callback) {
    const std::string endpoint = "/api/users/" + std::to_string(userId) + "/notifications/mark-seen";
    
    httpClient->put(endpoint, "{}", [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to mark notifications as seen"
            );
            callback(false, errorMessage);
            return;
        }
        
        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage);
            return;
        }
        
        callback(true, response.value("message", "Notifications marked as seen"));
    });
}

void NotificationHandler::getNotificationSettings(unsigned int userId, SettingsCallback callback) {
    const std::string endpoint = "/api/users/" + std::to_string(userId) + "/notification-settings";
    
    httpClient->get(endpoint, [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to fetch notification settings"
            );
            callback(false, errorMessage, {});
            return;
        }
        
        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage, {});
            return;
        }
        
        try {
            std::vector<NotificationSetting*> settings;
            
            for (const auto& settingJson : response["data"]) {
                NotificationSetting* setting = new NotificationSetting(NotificationSetting::fromJson(settingJson));
                settings.push_back(setting);
            }
            
            callback(true, response.value("message", "Settings retrieved successfully"), settings);
        } catch (const std::exception& exception) {
            callback(false, "Error processing settings data: " + std::string(exception.what()), {});
        }
    });
}

void NotificationHandler::updateNotificationSetting(unsigned int userId, unsigned int categoryId, bool enabled, StatusCallback callback) {
    const std::string endpoint = "/api/users/" + std::to_string(userId) + "/notification-settings";
    
    nlohmann::json requestBody = {
        {"categoryId", categoryId},
        {"enabled", enabled}
    };
    
    httpClient->post(endpoint, requestBody.dump(), [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {400, "Invalid settings data"},
                {404, "Category not found"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to update notification setting", 
                statusMessages
            );
            callback(false, errorMessage);
            return;
        }
        
        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage);
            return;
        }
        
        callback(true, response.value("message", "Notification setting updated successfully"));
    });
}

void NotificationHandler::getKeywords(unsigned int userId, KeywordsCallback callback) {
    const std::string endpoint = "/api/users/" + std::to_string(userId) + "/keywords";
    
    httpClient->get(endpoint, [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to fetch keywords"
            );
            callback(false, errorMessage, {});
            return;
        }
        
        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage, {});
            return;
        }
        
        try {
            std::vector<std::string> keywords;
            
            for (const auto& keywordJson : response["data"]) {
                if (keywordJson.contains("keyword")) {
                    keywords.push_back(keywordJson["keyword"].get<std::string>());
                }
            }
            
            callback(true, response.value("message", "Keywords retrieved successfully"), keywords);
        } catch (const std::exception& exception) {
            callback(false, "Error processing keywords data: " + std::string(exception.what()), {});
        }
    });
}

void NotificationHandler::addKeyword(unsigned int userId, const std::string& keyword, StatusCallback callback) {
    const std::string endpoint = "/api/users/" + std::to_string(userId) + "/keywords";
    
    nlohmann::json requestBody = {
        {"keyword", keyword}
    };
    
    httpClient->post(endpoint, requestBody.dump(), [this, callback, keyword](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {400, "Invalid keyword format"},
                {409, "Keyword '" + keyword + "' already exists"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to add keyword", 
                statusMessages
            );
            callback(false, errorMessage);
            return;
        }
        
        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage);
            return;
        }
        
        callback(true, response.value("message", "Keyword added successfully"));
    });
}

void NotificationHandler::removeKeyword(unsigned int userId, const std::string& keyword, StatusCallback callback) {
    const std::string endpoint = "/api/users/" + std::to_string(userId) + "/keywords/" + httpClient->urlEncode(keyword);
    
    httpClient->del(endpoint, [this, callback, keyword](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {404, "Keyword '" + keyword + "' not found"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to remove keyword", 
                statusMessages
            );
            callback(false, errorMessage);
            return;
        }
        
        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage);
            return;
        }
        
        callback(true, response.value("message", "Keyword removed successfully"));
    });
}
