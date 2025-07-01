#include "../Inc/TheNewsApi.h"
#include "../../Dao/Inc/ArticleDao.h"
#include "../../Dao/Inc/CategoryDao.h"
#include "../../Dao/Inc/ExternalServerDao.h"
#include "../../../Common/Inc/httplib.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <regex>

TheNewsApi::TheNewsApi() 
    : apiUrl("https://api.thenewsapi.com/v1/news/top"), 
      active(false), 
      running(false) {
    
    articleDao = std::make_unique<ArticleDao>();
    categoryDao = std::make_unique<CategoryDao>();
}

TheNewsApi::~TheNewsApi() {
    stopAutoFetch();
}

std::vector<Article> TheNewsApi::fetchNews() {
    std::vector<Article> articles;
    
    if (!active || apiKey.empty()) {
        std::cerr << "TheNewsApi is not active or API key is missing" << std::endl;
        return articles;
    }
    
    try {
        updateLastAccessed();
        httplib::SSLClient cli("api.thenewsapi.com");
        cli.set_connection_timeout(5);
        cli.enable_server_certificate_verification(false);
        std::string path = "/v1/news/top?api_token=" + apiKey + "&locale=us&limit=3";
        auto res = cli.Get(path.c_str());
        
        if (res) {
            if (res->status == 200) {
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
            } else {
                std::cerr << "Error fetching news. Status: " << res->status << std::endl;
            }
        } else {
            auto err = res.error();
            std::cerr << "Error fetching news: " << httplib::to_string(err) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in fetchNews: " << e.what() << std::endl;
    }
    
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
    
    try {
        ExternalServerDao serverDao;
        auto server = serverDao.findByName(getName());
        if (server) {
            serverDao.setApiStatus(server->apiId, active ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error updating API status: " << e.what() << std::endl;
    }
}

void TheNewsApi::startAutoFetch(int intervalMinutes) {
    if (running) {
        std::cout << "Auto-fetch is already running." << std::endl;
        return;
    }
    
    running = true;
    fetchThread = std::thread(&TheNewsApi::autoFetchTask, this, intervalMinutes);
    fetchThread.detach(); 
}

void TheNewsApi::stopAutoFetch() {
    if (!running) {
        return;
    }
    
    {
        std::unique_lock<std::mutex> lock(mutex);
        running = false;
        cv.notify_one();
    }
    
    if (fetchThread.joinable()) {
        fetchThread.join();
    }
}

std::string TheNewsApi::buildApiUrl() const {
    return apiUrl + "?api_token=" + apiKey + "&locale=us&limit=3";
}

void TheNewsApi::autoFetchTask(int intervalMinutes) {
    while (running) {
        if (active) {
            std::cout << "Fetching news from TheNewsAPI..." << std::endl;
            auto articles = fetchNews();
            if (processAndStoreArticles(articles)) {
                std::cout << "Successfully fetched and stored " << articles.size() << " articles." << std::endl;
            } else {
                std::cerr << "Failed to process some articles." << std::endl;
            }
        }
        
        std::unique_lock<std::mutex> lock(mutex);
        auto waitUntil = std::chrono::system_clock::now();
        if (intervalMinutes > 0) {
            waitUntil += std::chrono::minutes(intervalMinutes);
        } else {
            waitUntil += std::chrono::minutes(60);
        }
        
        cv.wait_until(lock, waitUntil, [this]() { return !running; });
    }
    
    std::cout << "News auto-fetch stopped." << std::endl;
}

bool TheNewsApi::processAndStoreArticles(const std::vector<Article>& articles) {
    bool allSuccessful = true;
    
    for (const auto& article : articles) {
        if (articleDao->articleExists(article.url)) {
            continue;
        }
        
        if (!articleDao->createArticle(article)) {
            std::cerr << "Failed to store article: " << article.title << std::endl;
            allSuccessful = false;
        }
    }
    
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
        std::cerr << "Error updating last accessed time: " << e.what() << std::endl;
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
