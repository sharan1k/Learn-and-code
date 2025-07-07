#include "../Inc/TheNewsApi.h"
#include "../../Dao/Inc/ArticleDao.h"
#include "../../Dao/Inc/CategoryDao.h"
#include "../../Dao/Inc/ExternalServerDao.h"
#include "../../Service/Inc/NotificationService.h"
#include "../../Utils/Inc/Logger.h"
#include "../../../Common/Inc/httplib.h"
#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <regex>
#include <map>

TheNewsApi::TheNewsApi() 
    : apiUrl("https://api.thenewsapi.com/v1/news/top"), 
      active(false) {
    
    articleDao = std::make_unique<ArticleDao>();
    categoryDao = std::make_unique<CategoryDao>();
}

TheNewsApi::~TheNewsApi() {
}

std::vector<Article> TheNewsApi::fetchNews() {
    std::vector<Article> articles;
    
    if (!active || apiKey.empty()) {
        Logger::warning("TheNewsApi is not active or API key is missing");
        return articles;
    }
    
    try {
        updateLastAccessed();
        Logger::debug("Setting up HTTP client for TheNewsApi");
        
        httplib::SSLClient cli("api.thenewsapi.com");
        cli.set_connection_timeout(5); 
        cli.set_read_timeout(10);
        cli.set_write_timeout(10);
        cli.enable_server_certificate_verification(false);
        
        std::string path = "/v1/news/top?api_token=" + apiKey + "&locale=us&limit=3";
        Logger::info("Fetching news from TheNewsApi");
        
        auto res = cli.Get(path.c_str());
        
        if (res && res->status == 200) {
            nlohmann::json response = nlohmann::json::parse(res->body);
            
            if (response.contains("data") && response["data"].is_array()) {
                for (const auto& item : response["data"]) {
                    Article article;
                    if (item.contains("title")) article.title = item["title"].get<std::string>();
                    
                    if (item.contains("description")) article.description = item["description"].get<std::string>();
                    else if (item.contains("snippet")) article.description = item["snippet"].get<std::string>();
                    
                    if (item.contains("source")) article.source = item["source"].get<std::string>();
                    
                    if (item.contains("url")) article.url = item["url"].get<std::string>();
        
                    std::string categoryName = "General";
                    if (item.contains("categories") && !item["categories"].empty()) {
                        categoryName = item["categories"][0].get<std::string>();
                    }
                    
                    article.categoryId = categoryDao->findOrCreateCategory(categoryName);
                    
                    if (item.contains("published_at")) {
                        std::string isoDateTime = item["published_at"].get<std::string>();
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
        }
    } catch (const std::exception& e) {
        Logger::error("Exception in TheNewsApi fetchNews: " + std::string(e.what()));
    } catch (...) {
        Logger::error("Unknown exception in TheNewsApi fetchNews");
    }
    
    Logger::info("TheNewsApi fetch returning " + std::to_string(articles.size()) + " articles");
    return articles;
}

bool TheNewsApi::initialize(const std::string& key) {
    apiKey = key;
    active = !apiKey.empty();
    return active;
}

std::string TheNewsApi::getName() const {
    return "TheNewsApi";
}

bool TheNewsApi::isActive() const {
    return active;
}

void TheNewsApi::setActive(bool isActive) {
    active = isActive;
    Logger::info("Setting TheNewsApi status to " + std::string(active ? "active" : "inactive"));
    
    try {
        ExternalServerDao serverDao;
        auto server = serverDao.findByName(getName());
        if (server) {
            serverDao.setApiStatus(server->apiId, active ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE);
        }
    } catch (const std::exception& e) {
        Logger::error("Error updating API status for TheNewsApi: " + std::string(e.what()));
    }
}

bool TheNewsApi::processAndStoreArticles(const std::vector<Article>& articles) {
    bool allSuccessful = true;
    NotificationService notificationService;
    int newArticles = 0;
    int existingArticles = 0;
    
    Logger::info("TheNewsApi processing " + std::to_string(articles.size()) + " articles");
    std::map<unsigned int, std::vector<unsigned int>> userNotifications;
    
    for (const auto& article : articles) {
        try {
            if (articleDao->articleExists(article.url)) {
                existingArticles++;
                continue;
            }
            
            unsigned int articleId = 0;
            if (articleDao->createArticle(article, &articleId)) {
                newArticles++;
                if (articleId > 0) {
                    std::vector<unsigned int> interestedUsers = notificationService.getUsersInterestedInArticle(articleId);
                    
                    for (unsigned int userId : interestedUsers) {
                        userNotifications[userId].push_back(articleId);
                        notificationService.createNotification(userId, articleId);
                    }
                }
            } else {
                Logger::error("Failed to store article from TheNewsApi: " + article.title);
                allSuccessful = false;
            }
        } catch (const std::exception& e) {
            Logger::error("Exception while processing article from TheNewsApi: " + std::string(e.what()));
            allSuccessful = false;
        }
    }
    
    for (const auto& [userId, articleIds] : userNotifications) {
        if (!articleIds.empty()) {
            std::vector<std::shared_ptr<Notification>> notifications;
            
            for (unsigned int articleId : articleIds) {
                auto notification = std::make_shared<Notification>();
                notification->userId = userId;
                notification->articleId = articleId;
                notification->seenStatus = false;
                notifications.push_back(notification);
            }
            
            notificationService.sendEmailNotification(userId, notifications);
        }
    }
    
    Logger::info("TheNewsApi processing complete: " + std::to_string(newArticles) + " new articles stored, " 
              + std::to_string(existingArticles) + " already existed");
    
    return allSuccessful;
}

void TheNewsApi::updateLastAccessed() {
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
        Logger::error("Error updating last accessed time for TheNewsApi: " + std::string(e.what()));
    }
}

std::string TheNewsApi::convertIsoToMySqlDateTime(const std::string& isoDateTime) {
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
