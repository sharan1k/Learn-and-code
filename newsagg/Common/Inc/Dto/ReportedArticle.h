#pragma once

#include <string>
#include <nlohmann/json.hpp>

enum class SeenStatus {
    SEEN,
    UNSEEN
};

struct ReportedArticle {
    unsigned int userId;
    unsigned int articleId;
    SeenStatus seenStatus;

    ReportedArticle() : userId(0), articleId(0), seenStatus(SeenStatus::UNSEEN) {}

    ReportedArticle(unsigned int _userId, unsigned int _articleId, SeenStatus _seenStatus = SeenStatus::UNSEEN)
        : userId(_userId),
          articleId(_articleId),
          seenStatus(_seenStatus) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["userId"] = userId;
        jsonData["articleId"] = articleId;
        jsonData["seenStatus"] = (seenStatus == SeenStatus::SEEN) ? "seen" : "unseen";
        return jsonData;
    }

    static ReportedArticle fromJson(const nlohmann::json& jsonData) {
        ReportedArticle reportedArticle;

        if (jsonData.contains("userId") && !jsonData["userId"].is_null())
            reportedArticle.userId = jsonData["userId"].get<unsigned int>();
        
        if (jsonData.contains("articleId") && !jsonData["articleId"].is_null())
            reportedArticle.articleId = jsonData["articleId"].get<unsigned int>();
        
        if (jsonData.contains("seenStatus") && !jsonData["seenStatus"].is_null()) {
            std::string status = jsonData["seenStatus"].get<std::string>();
            reportedArticle.seenStatus = (status == "seen") ? SeenStatus::SEEN : SeenStatus::UNSEEN;
        }
        
        return reportedArticle;
    }

    static std::string seenStatusToString(SeenStatus status) {
        return (status == SeenStatus::SEEN) ? "seen" : "unseen";
    }

    static SeenStatus stringToSeenStatus(const std::string& status) {
        return (status == "seen") ? SeenStatus::SEEN : SeenStatus::UNSEEN;
    }
};
