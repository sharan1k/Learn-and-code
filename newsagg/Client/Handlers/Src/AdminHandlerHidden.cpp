#include "../Inc/AdminHandler.h"
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

void AdminHandler::getReportedArticles(ArticleListCallback callback) {
    client->get("/api/admin/reported-articles", [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to retrieve reported articles"
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
        
        std::vector<Article*> articles;
        
        try {
            for (const auto& articleJson : response["data"]) {
                Article* article = new Article(Article::fromJson(articleJson));
                articles.push_back(article);
            }
            
            callback(true, "Successfully retrieved reported articles", articles);
        } catch (const std::exception& e) {
            for (auto* article : articles) {
                delete article;
            }
            callback(false, "Error processing article data: " + std::string(e.what()), {});
        }
    });
}

void AdminHandler::hideArticle(unsigned int articleId, StatusCallback callback) {
    const std::string endpoint = "/api/admin/articles/" + std::to_string(articleId) + "/hide?id=" + std::to_string(articleId);
    
    client->post(endpoint, "", [this, callback, articleId](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {404, "Article not found"},
                {409, "Article is already hidden"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to hide article", 
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
        
        callback(true, "Article hidden successfully");
    });
}

void AdminHandler::unhideArticle(unsigned int articleId, StatusCallback callback) {
    const std::string endpoint = "/api/admin/articles/" + std::to_string(articleId) + "/unhide?id=" + std::to_string(articleId);
    
    client->post(endpoint, "", [this, callback, articleId](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {404, "Article not found"},
                {409, "Article is not currently hidden"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to unhide article", 
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
        
        callback(true, "Article unhidden successfully");
    });
}

void AdminHandler::hideCategory(unsigned int categoryId, StatusCallback callback) {
    const std::string endpoint = "/api/admin/categories/" + std::to_string(categoryId) + "/hide?id=" + std::to_string(categoryId);
    
    client->post(endpoint, "", [this, callback, categoryId](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {404, "Category not found"},
                {409, "Category is already hidden"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to hide category", 
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
        
        callback(true, "Category hidden successfully");
    });
}

void AdminHandler::unhideCategory(unsigned int categoryId, StatusCallback callback) {
    const std::string endpoint = "/api/admin/categories/" + std::to_string(categoryId) + "/unhide?id=" + std::to_string(categoryId);
    
    client->post(endpoint, "", [this, callback, categoryId](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {404, "Category not found"},
                {409, "Category is not currently hidden"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to unhide category", 
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
        
        callback(true, "Category unhidden successfully");
    });
}

void AdminHandler::getHiddenArticles(IdListCallback callback) {
    client->get("/api/admin/hidden-articles", [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to retrieve hidden articles"
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
            std::vector<unsigned int> hiddenArticleIds;
            
            for (const auto& idJson : response["data"]) {
                if (idJson.contains("articleId")) {
                    hiddenArticleIds.push_back(idJson["articleId"].get<unsigned int>());
                }
            }
            
            callback(true, "Successfully retrieved hidden articles", hiddenArticleIds);
        } catch (const std::exception& e) {
            callback(false, "Error processing hidden article data: " + std::string(e.what()), {});
        }
    });
}

void AdminHandler::getHiddenCategories(IdListCallback callback) {
    client->get("/api/admin/hidden-categories", [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to retrieve hidden categories"
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
            std::vector<unsigned int> hiddenCategoryIds;
            
            for (const auto& idJson : response["data"]) {
                if (idJson.contains("categoryId")) {
                    hiddenCategoryIds.push_back(idJson["categoryId"].get<unsigned int>());
                }
            }
            
            callback(true, "Successfully retrieved hidden categories", hiddenCategoryIds);
        } catch (const std::exception& e) {
            callback(false, "Error processing hidden category data: " + std::string(e.what()), {});
        }
    });
}
