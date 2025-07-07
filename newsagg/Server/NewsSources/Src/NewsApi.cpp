#include "../Inc/NewsApi.h"
#include "../../Dao/Inc/ArticleDao.h"
#include "../../Dao/Inc/CategoryDao.h"
#include "../../Dao/Inc/ExternalServerDao.h"
#include "../../Service/Inc/NotificationService.h"
#include "../../../Common/Inc/httplib.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <regex>

NewsApi::NewsApi() 
    : apiUrl("https://newsapi.org/v2/top-headlines"), 
      active(false) {
    
    articleDao = std::make_unique<ArticleDao>();
    categoryDao = std::make_unique<CategoryDao>();
}

NewsApi::~NewsApi() {
}

std::vector<Article> NewsApi::fetchNews() {
    std::vector<Article> articles;
    
    if (!active || apiKey.empty()) {
        std::cerr << "NewsApi is not active or API key is missing" << std::endl;
        return articles;
    }
    
    try {
        updateLastAccessed();
        httplib::SSLClient cli("newsapi.org");
        cli.set_connection_timeout(5);
        cli.enable_server_certificate_verification(false);
        std::string path = "/v2/top-headlines?country=us&category=technology&apiKey=" + apiKey;
        auto res = cli.Get(path.c_str());
        
        if (res) {
            if (res->status == 200) {
                nlohmann::json response = nlohmann::json::parse(res->body);
                
                if (response.contains("articles") && response["articles"].is_array()) {
                    for (const auto& item : response["articles"]) {
                        Article article;
                        
                        if (item.contains("title")) article.title = item["title"].get<std::string>();
                        
                        if (item.contains("description") && !item["description"].is_null()) {
                            article.description = item["description"].get<std::string>();
                        } else {
                            article.description = "No description available";
                        }
                        
                        if (item.contains("source") && item["source"].contains("name")) {
                            article.source = item["source"]["name"].get<std::string>();
                        } else {
                            article.source = "NewsAPI";
                        }
                        
                        if (item.contains("url")) article.url = item["url"].get<std::string>();
            
                        std::string categoryName = "Business";
                        article.categoryId = categoryDao->findOrCreateCategory(categoryName);
                        
                        if (item.contains("publishedAt")) {
                            std::string isoDateTime = item["publishedAt"].get<std::string>();
                            article.publishedAt = convertIsoToMySqlDateTime(isoDateTime);
                        } else {
                            auto now = std::chrono::system_clock::now();
                            auto in_time_t = std::chrono::system_clock::to_time_t(now);
                            std::stringstream ss;
                            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
                            article.publishedAt = ss.str();
                        }
                        
                        articles.push_back(article);
                    }
                }
            } else {
                std::cerr << "Error fetching news from NewsAPI. Status: " << res->status << std::endl;
                if (res->body.find("message") != std::string::npos) {
                    nlohmann::json errorJson = nlohmann::json::parse(res->body);
                    if (errorJson.contains("message")) {
                        std::cerr << "Error message: " << errorJson["message"].get<std::string>() << std::endl;
                    }
                }
            }
        } else {
            auto err = res.error();
            std::cerr << "Error fetching news from NewsAPI: " << httplib::to_string(err) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in fetchNews for NewsAPI: " << e.what() << std::endl;
    }
    
    return articles;
}

bool NewsApi::initialize(const std::string& key) {
    apiKey = key;
    active = !apiKey.empty();
    return active;
}

std::string NewsApi::getName() const {
    return "NewsApi";
}

bool NewsApi::isActive() const {
    return active;
}

void NewsApi::setActive(bool isActive) {
    active = isActive;
    
    try {
        ExternalServerDao serverDao;
        auto server = serverDao.findByName(getName());
        if (server) {
            serverDao.setApiStatus(server->apiId, active ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error updating API status for NewsAPI: " << e.what() << std::endl;
    }
}

bool NewsApi::processAndStoreArticles(const std::vector<Article>& articles) {
    bool allSuccessful = true;
    NotificationService notificationService;
    
    for (const auto& article : articles) {
        if (articleDao->articleExists(article.url)) {
            continue;
        }
        
        unsigned int articleId = 0;
        if (articleDao->createArticle(article, &articleId)) {
            if (articleId > 0) {
                notificationService.processArticleForNotifications(articleId);
            }
        } else {
            std::cerr << "Failed to store article from NewsAPI: " << article.title << std::endl;
            allSuccessful = false;
        }
    }
    
    return allSuccessful;
}

void NewsApi::updateLastAccessed() {
    try {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        std::string timestamp = ss.str();
        
        ExternalServerDao serverDao;
        auto server = serverDao.findByName(getName());
        if (server) {
            serverDao.updateLastAccessed(server->apiId, timestamp);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error updating last accessed time for NewsAPI: " << e.what() << std::endl;
    }
}

std::string NewsApi::convertIsoToMySqlDateTime(const std::string& isoDateTime) {
    std::regex dateTimeRegex("(\\d{4}-\\d{2}-\\d{2})T(\\d{2}:\\d{2}:\\d{2})");
    std::smatch matches;
    
    if (std::regex_search(isoDateTime, matches, dateTimeRegex) && matches.size() >= 3) {
        return matches[1].str() + " " + matches[2].str();
    }
    
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
