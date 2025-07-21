#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct HiddenArticle {
    unsigned int articleId;

    HiddenArticle() : articleId(0) {}

    HiddenArticle(unsigned int _articleId)
        : articleId(_articleId) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["articleId"] = articleId;
        return jsonData;
    }

    static HiddenArticle fromJson(const nlohmann::json& jsonData) {
        HiddenArticle hiddenArticle;

        if (jsonData.contains("articleId") && !jsonData["articleId"].is_null())
            hiddenArticle.articleId = jsonData["articleId"].get<unsigned int>();
        
        return hiddenArticle;
    }
};
