#pragma once

#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

struct Article {
    unsigned int articleId;
    std::string title;
    std::string description;
    std::string source;
    std::string url;
    unsigned int categoryId;
    std::string publishedAt;  // Using string for datetime to simplify JSON serialization

    Article() : articleId(0), categoryId(0) {}

    Article(const std::string& _title, const std::string& _description, const std::string& _source, 
            const std::string& _url, unsigned int _categoryId, const std::string& _publishedAt)
        : articleId(0), 
          title(_title), 
          description(_description), 
          source(_source), 
          url(_url), 
          categoryId(_categoryId), 
          publishedAt(_publishedAt) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["articleId"] = articleId;
        jsonData["title"] = title;
        jsonData["description"] = description;
        jsonData["source"] = source;
        jsonData["url"] = url;
        jsonData["categoryId"] = categoryId;
        jsonData["publishedAt"] = publishedAt;
        return jsonData;
    }

    static Article fromJson(const nlohmann::json& jsonData) {
        Article article;

        if (jsonData.contains("articleId") && !jsonData["articleId"].is_null())
            article.articleId = jsonData["articleId"].get<unsigned int>();
        
        if (jsonData.contains("title") && !jsonData["title"].is_null())
            article.title = jsonData["title"].get<std::string>();
        
        if (jsonData.contains("description") && !jsonData["description"].is_null())
            article.description = jsonData["description"].get<std::string>();
        
        if (jsonData.contains("source") && !jsonData["source"].is_null())
            article.source = jsonData["source"].get<std::string>();
        
        if (jsonData.contains("url") && !jsonData["url"].is_null())
            article.url = jsonData["url"].get<std::string>();
        
        if (jsonData.contains("categoryId") && !jsonData["categoryId"].is_null())
            article.categoryId = jsonData["categoryId"].get<unsigned int>();
        
        if (jsonData.contains("publishedAt") && !jsonData["publishedAt"].is_null())
            article.publishedAt = jsonData["publishedAt"].get<std::string>();
        
        return article;
    }

    bool isValid() const {
        return !title.empty() && !url.empty() && categoryId > 0;
    }
};
