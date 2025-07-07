#include "../Inc/ArticleHandler.h"
#include <nlohmann/json.hpp>

void ArticleHandler::reportArticle(unsigned int userId, unsigned int articleId, StatusCallback callback) {
    std::string url = "/api/articles/" + std::to_string(articleId) + "/report?userId=" + std::to_string(userId);    
    nlohmann::json requestBody;
    requestBody["userId"] = userId;
    std::string requestBodyStr = requestBody.dump();
    
    httpClient->post(url, requestBodyStr, [this, callback, articleId, userId, requestBodyStr](const httplib::Result& result) {
        bool success = false;
        std::string errorMessage = "Failed to report article";
        
        if (result) {
            if (result->status == 404) {
                std::string fallbackUrl = "/api/articles/report/" + std::to_string(articleId) + "?userId=" + std::to_string(userId);
                
                httpClient->post(fallbackUrl, requestBodyStr, [this, callback](const httplib::Result& fallbackResult) {
                    bool fallbackSuccess = false;
                    std::string fallbackErrorMessage = "Failed to report article";
                    
                    if (fallbackResult && fallbackResult->status == 200) {
                        try {
                            nlohmann::json response = parseResponse(fallbackResult, fallbackSuccess, fallbackErrorMessage);
                            if (fallbackSuccess && callback) {
                                std::string message = "Article reported successfully";
                                if (response.contains("message") && response["message"].is_string()) {
                                    message = response["message"];
                                }
                                callback(true, message);
                                return;
                            }
                        } catch (...) {
                            callback(false, "Article reporting feature may not be available yet");
                            return;
                        }
                    }
                    
                    callback(false, "Article reporting feature may not be available yet");
                });
                return;
            }
            
            if (result->status == 200 || result->status == 201) {
                try {
                    nlohmann::json response = nlohmann::json::parse(result->body);
                    std::string message = "Article reported successfully";
                    
                    if (response.contains("message") && response["message"].is_string()) {
                        message = response["message"];
                    }
                    
                    callback(true, message);
                    return;
                } catch (...) {
                    callback(true, "Article reported successfully");
                    return;
                }
            }
            
            errorMessage = "Server returned status code: " + std::to_string(result->status);
            if (!result->body.empty()) {
                errorMessage += " with body: " + result->body.substr(0, 100);
                if (result->body.length() > 100) errorMessage += "...";
            }
        } else {
            errorMessage = "Network error";
        }
        
        if (callback) {
            callback(success, errorMessage);
        }
    });
}
