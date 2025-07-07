#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct NotificationSetting {
    unsigned int userId;
    unsigned int categoryId;

    NotificationSetting() : userId(0), categoryId(0) {}

    NotificationSetting(unsigned int _userId, unsigned int _categoryId)
        : userId(_userId), 
          categoryId(_categoryId) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["userId"] = userId;
        jsonData["categoryId"] = categoryId;
        return jsonData;
    }

    static NotificationSetting fromJson(const nlohmann::json& jsonData) {
        NotificationSetting setting;
        
        if (jsonData.contains("userId") && !jsonData["userId"].is_null())
            setting.userId = jsonData["userId"].get<unsigned int>();
        
        if (jsonData.contains("categoryId") && !jsonData["categoryId"].is_null())
            setting.categoryId = jsonData["categoryId"].get<unsigned int>();
        
        return setting;
    }
};
