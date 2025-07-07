#include "../Inc/NotificationHandler.h"
#include <nlohmann/json.hpp>
#include <iostream>

NotificationHandler::NotificationHandler(std::shared_ptr<HttpClient> httpClient)
    : httpClient(httpClient) {
}

void NotificationHandler::getNotifications(unsigned int userId, NotificationsCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/notifications";
    
    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<Notification*> notifications;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";
        
        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);
                
                if (responseJson["status"] == "success") {
                    for (const auto& notificationJson : responseJson["data"]) {
                        Notification* notification = new Notification(Notification::fromJson(notificationJson));
                        notifications.push_back(notification);
                    }
                    
                    message = responseJson["message"];
                    callback(true, message, notifications);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch notifications: " + response;
        }
        
        callback(false, message, notifications);
    };
    
    httpClient->get(endpoint, httpCallback);
}

void NotificationHandler::markNotificationsAsSeen(unsigned int userId, SimpleCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/notifications/mark-seen";
    
    auto httpCallback = [callback](const httplib::Result& result) {
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";
        
        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);
                
                if (responseJson["status"] == "success") {
                    message = responseJson["message"];
                    callback(true, message);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to mark notifications as seen: " + response;
        }
        
        callback(false, message);
    };
    
    httpClient->put(endpoint, "{}", httpCallback);
}

void NotificationHandler::getNotificationSettings(unsigned int userId, NotificationSettingsCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/notification-settings";
    
    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<NotificationSetting*> settings;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";
        
        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);
                
                if (responseJson["status"] == "success") {
                    for (const auto& settingJson : responseJson["data"]) {
                        NotificationSetting* setting = new NotificationSetting(NotificationSetting::fromJson(settingJson));
                        settings.push_back(setting);
                    }
                    
                    message = responseJson["message"];
                    callback(true, message, settings);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch notification settings: " + response;
        }
        
        callback(false, message, settings);
    };
    
    httpClient->get(endpoint, httpCallback);
}

void NotificationHandler::updateNotificationSetting(unsigned int userId, unsigned int categoryId, bool enabled, SimpleCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/notification-settings";
    
    nlohmann::json requestBody = {
        {"categoryId", categoryId},
        {"enabled", enabled}
    };
    
    auto httpCallback = [callback](const httplib::Result& result) {
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";
        
        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);
                
                if (responseJson["status"] == "success") {
                    message = responseJson["message"];
                    callback(true, message);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to update notification setting: " + response;
        }
        
        callback(false, message);
    };
    
    httpClient->post(endpoint, requestBody.dump(), httpCallback);
}

void NotificationHandler::getKeywords(unsigned int userId, KeywordsCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/keywords";
    
    auto httpCallback = [callback](const httplib::Result& result) {
        std::vector<std::string> keywords;
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";
        
        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);
                
                if (responseJson["status"] == "success") {
                    for (const auto& keywordJson : responseJson["data"]) {
                        if (keywordJson.contains("keyword")) {
                            keywords.push_back(keywordJson["keyword"].get<std::string>());
                        }
                    }
                    
                    message = responseJson["message"];
                    callback(true, message, keywords);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to fetch keywords: " + response;
        }
        
        callback(false, message, keywords);
    };
    
    httpClient->get(endpoint, httpCallback);
}

void NotificationHandler::addKeyword(unsigned int userId, const std::string& keyword, SimpleCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/keywords";
    
    nlohmann::json requestBody = {
        {"keyword", keyword}
    };
    
    auto httpCallback = [callback](const httplib::Result& result) {
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";
        
        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);
                
                if (responseJson["status"] == "success") {
                    message = responseJson["message"];
                    callback(true, message);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to add keyword: " + response;
        }
        
        callback(false, message);
    };
    
    httpClient->post(endpoint, requestBody.dump(), httpCallback);
}

void NotificationHandler::removeKeyword(unsigned int userId, const std::string& keyword, SimpleCallback callback) {
    std::string endpoint = "/api/users/" + std::to_string(userId) + "/keywords/" + httpClient->urlEncode(keyword);
    
    auto httpCallback = [callback](const httplib::Result& result) {
        std::string message;
        bool success = result && result->status == 200;
        std::string response = success ? result->body : "Failed to connect to server";
        
        if (success) {
            try {
                nlohmann::json responseJson = nlohmann::json::parse(response);
                
                if (responseJson["status"] == "success") {
                    message = responseJson["message"];
                    callback(true, message);
                    return;
                } else {
                    message = responseJson["message"];
                }
            } catch (const std::exception& e) {
                message = "Failed to parse response: " + std::string(e.what());
            }
        } else {
            message = "Failed to remove keyword: " + response;
        }
        
        callback(false, message);
    };
    
    httpClient->del(endpoint, httpCallback);
}
