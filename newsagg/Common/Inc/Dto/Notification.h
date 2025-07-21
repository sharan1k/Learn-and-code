#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct Notification {
    unsigned int userId;
    unsigned int articleId;
    std::string seenStatus;

    Notification() : userId(0), articleId(0), seenStatus("unseen") {}

    Notification(unsigned int _userId, unsigned int _articleId, const std::string& _seenStatus = "unseen")
        : userId(_userId), 
          articleId(_articleId),
          seenStatus(_seenStatus) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["userId"] = userId;
        jsonData["articleId"] = articleId;
        jsonData["seenStatus"] = seenStatus;
        return jsonData;
    }

    static Notification fromJson(const nlohmann::json& jsonData) {
        Notification notification;
        
        if (jsonData.contains("userId") && !jsonData["userId"].is_null())
            notification.userId = jsonData["userId"].get<unsigned int>();
        
        if (jsonData.contains("articleId") && !jsonData["articleId"].is_null())
            notification.articleId = jsonData["articleId"].get<unsigned int>();
        
        if (jsonData.contains("seenStatus") && !jsonData["seenStatus"].is_null())
            notification.seenStatus = jsonData["seenStatus"].get<std::string>();
        
        return notification;
    }
};
