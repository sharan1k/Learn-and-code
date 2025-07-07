#include "../Inc/NewsApi.h"
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
#include <thread> 
#include <ctime>
#include <regex>
#include <map>

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
        Logger::warning("NewsApi is not active or API key is missing");
        return articles;
    }
    
    std::vector<std::pair<std::string, std::string>> categories = {
        {"general", "General"},
        {"business", "Business"},
        {"entertainment", "Entertainment"},
        {"health", "Health"},
        {"science", "Science"},
        {"sports", "Sports"},
        {"technology", "Technology"}
    };
    
    try {
        updateLastAccessed();
        Logger::debug("Setting up HTTP client for NewsApi");
        httplib::SSLClient cli("newsapi.org");
        cli.set_connection_timeout(5);
        cli.enable_server_certificate_verification(false);
        
        for (const auto& category : categories) {
            std::string apiCategory = category.first;
            std::string displayCategory = category.second;
            Logger::info("Fetching news for category: " + displayCategory);
            
            std::string path = "/v2/top-headlines?country=us&category=" + apiCategory + "&apiKey=" + apiKey;
            auto res = cli.Get(path.c_str());
            
            if (res && res->status == 200) {
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
            
                        article.categoryId = categoryDao->findOrCreateCategory(apiCategory);
                        
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
                Logger::error("Error fetching news for category " + displayCategory + " from NewsAPI");
                if (res) {
                    Logger::error("Status: " + std::to_string(res->status));
                    if (res->body.find("message") != std::string::npos) {
                        try {
                            nlohmann::json errorJson = nlohmann::json::parse(res->body);
                            if (errorJson.contains("message")) {
                                Logger::error("Error message: " + errorJson["message"].get<std::string>());
                            }
                        } catch (const std::exception& e) {
                            Logger::error("Failed to parse error message: " + std::string(e.what()));
                        }
                    }
                } else {
                    auto err = res.error();
                    Logger::error("Error: " + httplib::to_string(err));
                }
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        Logger::error("Exception in fetchNews for NewsAPI: " + std::string(e.what()));
    }
    
    Logger::info("NewsAPI fetch returning " + std::to_string(articles.size()) + " articles");
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
    Logger::info("Setting NewsApi status to " + std::string(active ? "active" : "inactive"));
    
    try {
        ExternalServerDao serverDao;
        auto server = serverDao.findByName(getName());
        if (server) {
            serverDao.setApiStatus(server->apiId, active ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE);
        }
    } catch (const std::exception& e) {
        Logger::error("Error updating API status for NewsAPI: " + std::string(e.what()));
    }
}

bool NewsApi::processAndStoreArticles(const std::vector<Article>& articles) {
    bool allSuccessful = true;
    NotificationService notificationService;
    int newArticles = 0;
    int existingArticles = 0;
    
    Logger::info("NewsApi processing " + std::to_string(articles.size()) + " articles");
    std::map<unsigned int, std::vector<unsigned int>> userNotifications;
    
    for (const auto& article : articles) {
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
            Logger::error("Failed to store article from NewsAPI: " + article.title);
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
    
    Logger::info("NewsApi processing complete: " + std::to_string(newArticles) + " new articles stored, " 
              + std::to_string(existingArticles) + " already existed");
    
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
            Logger::debug("Updated last accessed time for NewsApi to " + timestamp);
        }
    } catch (const std::exception& e) {
        Logger::error("Error updating last accessed time for NewsApi: " + std::string(e.what()));
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
