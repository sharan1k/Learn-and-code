#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct Like {
    unsigned int userId;
    unsigned int articleId;
    std::string likeDislike; 

    Like() : userId(0), articleId(0), likeDislike("like") {}

    Like(unsigned int _userId, unsigned int _articleId, const std::string& _likeDislike = "like")
        : userId(_userId), 
          articleId(_articleId),
          likeDislike(_likeDislike) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["userId"] = userId;
        jsonData["articleId"] = articleId;
        jsonData["likeDislike"] = likeDislike;
        return jsonData;
    }

    static Like fromJson(const nlohmann::json& jsonData) {
        Like like;
        
        if (jsonData.contains("userId") && !jsonData["userId"].is_null())
            like.userId = jsonData["userId"].get<unsigned int>();
        
        if (jsonData.contains("articleId") && !jsonData["articleId"].is_null())
            like.articleId = jsonData["articleId"].get<unsigned int>();
        
        if (jsonData.contains("likeDislike") && !jsonData["likeDislike"].is_null())
            like.likeDislike = jsonData["likeDislike"].get<std::string>();
        
        return like;
    }
};
