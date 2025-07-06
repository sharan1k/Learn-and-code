#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct SavedArticle {
    unsigned int userId;
    unsigned int articleId;

    SavedArticle() : userId(0), articleId(0) {}

    SavedArticle(unsigned int _userId, unsigned int _articleId)
        : userId(_userId), 
          articleId(_articleId) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["userId"] = userId;
        jsonData["articleId"] = articleId;
        return jsonData;
    }

    static SavedArticle fromJson(const nlohmann::json& jsonData) {
        SavedArticle savedArticle;
        
        if (jsonData.contains("userId") && !jsonData["userId"].is_null())
            savedArticle.userId = jsonData["userId"].get<unsigned int>();
        
        if (jsonData.contains("articleId") && !jsonData["articleId"].is_null())
            savedArticle.articleId = jsonData["articleId"].get<unsigned int>();
        
        return savedArticle;
    }
};
