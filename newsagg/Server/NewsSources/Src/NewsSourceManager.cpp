#include "../Inc/NewsSourceManager.h"
#include "../Inc/TheNewsApi.h"
#include "../Inc/NewsApi.h"
#include "../../Utils/Inc/Logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

NewsSourceManager::NewsSourceManager() : isFetching(false) {
}

NewsSourceManager::~NewsSourceManager() {
    stopFetchingNews();
}

NewsSourceManager& NewsSourceManager::getInstance() {
    static NewsSourceManager instance;
    return instance;
}

bool NewsSourceManager::initializeNewsSource(std::shared_ptr<INewsSource> newsSource, const std::string& apiKey) {
    if (!newsSource) {
        Logger::error("Cannot initialize null news source");
        return false;
    }
    
    const std::string& sourceName = newsSource->getName();
    
    if (!newsSource->initialize(apiKey)) {
        Logger::error("Failed to initialize news source '" + sourceName + "'");
        return false;
    }
    
    Logger::info("News source '" + sourceName + "' initialized");
    return true;
}

void NewsSourceManager::startFetchingNews(int intervalMinutes) {
    if (isFetching) {
        Logger::info("News auto-fetch is already running. Stopping existing fetch before starting new one");
        stopFetchingNews();
    }
    
    std::string timeMessage;
    if (intervalMinutes >= 60 && intervalMinutes % 60 == 0) {
        int hours = intervalMinutes / 60;
        timeMessage = std::to_string(hours) + "-hour" + (hours > 1 ? "s" : "");
    } else {
        timeMessage = std::to_string(intervalMinutes) + "-minute" + (intervalMinutes > 1 ? "s" : "");
    }
    
    Logger::info("Starting sequential news fetching with " + timeMessage + " interval");
    
    isFetching = true;
    fetchThread = std::thread([this, intervalMinutes]() {
        this->fetchNewsSequentially();
        
        while (isFetching) {
            std::this_thread::sleep_for(std::chrono::minutes(intervalMinutes));
            
            if (isFetching) {
                this->fetchNewsSequentially();
            }
        }
    });
    fetchThread.detach();
    
    Logger::info("News auto-fetch started with " + timeMessage + " interval for all active sources");
}

void NewsSourceManager::stopFetchingNews() {
    if (isFetching) {
        isFetching = false;
                  
        if (fetchThread.joinable()) {
            fetchThread.join();
        }
        
        Logger::info("Stopped news auto-fetch");
    }
}

std::shared_ptr<INewsSource> NewsSourceManager::getNewsSource(const std::string& name) {
    std::lock_guard<std::mutex> lock(sourcesMutex);
    
    auto it = newsSources.find(name);
    if (it != newsSources.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<INewsSource>> NewsSourceManager::getAllNewsSources() {
    std::vector<std::shared_ptr<INewsSource>> sources;
    
    std::lock_guard<std::mutex> lock(sourcesMutex);
    for (const auto& pair : newsSources) {
        sources.push_back(pair.second);
    }
    
    return sources;
}

void NewsSourceManager::loadNewsSourcesFromDatabase() {
    Logger::info("Loading news sources from database");
    auto servers = serverDao.getAllExternalServers();
    
    for (const auto& server : servers) {
        {
            std::lock_guard<std::mutex> lock(sourcesMutex);
            if (newsSources.find(server->apiName) != newsSources.end()) {
                Logger::info("News source '" + server->apiName + "' is already loaded");
                continue;
            }
        }
        
        auto newsSource = createNewsSource(server->apiName);
        if (newsSource) {
            if (initializeNewsSource(newsSource, server->apiKey)) {
                newsSource->setActive(server->apiStatus == ApiStatus::ACTIVE);
                
                std::lock_guard<std::mutex> lock(sourcesMutex);
                newsSources[server->apiName] = newsSource;
                Logger::info("Loaded news source '" + server->apiName + "' from database");
            }
        } else {
            Logger::error("Unknown news source type: '" + server->apiName + "'");
        }
    }
}

bool NewsSourceManager::fetchNewsNow() {
    Logger::info("Manually fetching news from all sources sequentially");
    return fetchNewsSequentially();
}

std::shared_ptr<INewsSource> NewsSourceManager::createNewsSource(const std::string& name) {
    if (name == "TheNewsApi") {
        return std::make_shared<TheNewsApi>();
    } else if (name == "NewsApi") {
        return std::make_shared<NewsApi>();
    }
    
    return nullptr;
}

bool NewsSourceManager::fetchNewsSequentially() {
    bool overallSuccess = true;
    
    {
        std::shared_ptr<INewsSource> newsApi = getNewsSource("NewsApi");
        if (newsApi && newsApi->isActive()) {
            try {
                Logger::info("Sequentially fetching news from source 'NewsApi'");
                
                auto articles = newsApi->fetchNews();
                Logger::info("Fetched " + std::to_string(articles.size()) + " articles from 'NewsApi'");
                
                if (auto api = std::dynamic_pointer_cast<NewsApi>(newsApi)) {
                    if (api->processAndStoreArticles(articles)) {
                        Logger::info("Successfully processed and stored articles from 'NewsApi'");
                    } else {
                        Logger::error("Failed to process some articles from 'NewsApi'");
                        overallSuccess = false;
                    }
                }
            } catch (const std::exception& exception) {
                Logger::error("Error fetching news from source 'NewsApi': " + std::string(exception.what()));
                overallSuccess = false;
            } catch (...) {
                Logger::error("Unknown error fetching news from source 'NewsApi'");
                overallSuccess = false;
            }
        }
    }
    
    {
        std::shared_ptr<INewsSource> theNewsApi = getNewsSource("TheNewsApi");
        if (theNewsApi && theNewsApi->isActive()) {
            try {
                Logger::info("Sequentially fetching news from source 'TheNewsApi'");
                auto articles = theNewsApi->fetchNews();
                Logger::info("Fetched " + std::to_string(articles.size()) + " articles from 'TheNewsApi'");
                
                if (auto api = std::dynamic_pointer_cast<TheNewsApi>(theNewsApi)) {
                    if (api->processAndStoreArticles(articles)) {
                        Logger::info("Successfully processed and stored articles from 'TheNewsApi'");
                    } else {
                        Logger::error("Failed to process some articles from 'TheNewsApi'");
                        overallSuccess = false;
                    }
                }
            } catch (const std::exception& exception) {
                Logger::error("Error fetching news from source 'TheNewsApi': " + std::string(exception.what()));
                overallSuccess = false;
            } catch (...) {
                Logger::error("Unknown error fetching news from source 'TheNewsApi'");
                overallSuccess = false;
            }
        }
    }
    
    Logger::info("Sequential fetching complete");
    return overallSuccess;
}
