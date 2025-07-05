#include "../Inc/NewsSourceManager.h"
#include "../Inc/TheNewsApi.h"
#include "../Inc/NewsApi.h"
#include <iostream>
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
        std::cerr << "Error: Cannot initialize null news source" << std::endl;
        return false;
    }
    
    const std::string& sourceName = newsSource->getName();
    
    if (!newsSource->initialize(apiKey)) {
        std::cerr << "Failed to initialize news source '" << sourceName << "'." << std::endl;
        return false;
    }
    
    std::cout << "News source '" << sourceName << "' initialized." << std::endl;
    return true;
}

void NewsSourceManager::startFetchingNews(int intervalMinutes) {
    if (isFetching) {
        std::cout << "News auto-fetch is already running. Stopping existing fetch before starting new one." << std::endl;
        stopFetchingNews();
    }
    
    std::string timeMessage;
    if (intervalMinutes >= 60 && intervalMinutes % 60 == 0) {
        int hours = intervalMinutes / 60;
        timeMessage = std::to_string(hours) + "-hour" + (hours > 1 ? "s" : "");
    } else {
        timeMessage = std::to_string(intervalMinutes) + "-minute" + (intervalMinutes > 1 ? "s" : "");
    }
    
    std::cout << "Starting sequential news fetching with " << timeMessage << " interval..." << std::endl;
    
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
    
    std::cout << "News auto-fetch started with " << timeMessage << " interval for all active sources." << std::endl;
}

void NewsSourceManager::stopFetchingNews() {
    if (isFetching) {
        isFetching = false;
                  
        if (fetchThread.joinable()) {
            fetchThread.join();
        }
        
        std::cout << "Stopped news auto-fetch." << std::endl;
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
    auto servers = serverDao.getAllExternalServers();
    
    for (const auto& server : servers) {
        {
            std::lock_guard<std::mutex> lock(sourcesMutex);
            if (newsSources.find(server->apiName) != newsSources.end()) {
                std::cout << "News source '" << server->apiName << "' is already loaded." << std::endl;
                continue;
            }
        }
        
        auto newsSource = createNewsSource(server->apiName);
        if (newsSource) {
            if (initializeNewsSource(newsSource, server->apiKey)) {
                newsSource->setActive(server->apiStatus == ApiStatus::ACTIVE);
                
                std::lock_guard<std::mutex> lock(sourcesMutex);
                newsSources[server->apiName] = newsSource;
                std::cout << "Loaded news source '" << server->apiName << "' from database." << std::endl;
            }
        } else {
            std::cerr << "Unknown news source type: '" << server->apiName << "'." << std::endl;
        }
    }
}

bool NewsSourceManager::fetchNewsNow() {
    std::cout << "Manually fetching news from all sources sequentially..." << std::endl;
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
    std::vector<std::string> sourceOrder;
    
    {
        std::shared_ptr<INewsSource> newsApi = getNewsSource("NewsApi");
        if (newsApi && newsApi->isActive()) {
            try {
                std::cout << "Sequentially fetching news from source 'NewsApi'..." << std::endl;
                
                auto articles = newsApi->fetchNews();
                std::cout << "Fetched " << articles.size() << " articles from 'NewsApi'." << std::endl;
                
                if (auto api = std::dynamic_pointer_cast<NewsApi>(newsApi)) {
                    if (api->processAndStoreArticles(articles)) {
                        std::cout << "Successfully processed and stored articles from 'NewsApi'." << std::endl;
                    } else {
                        std::cerr << "Failed to process some articles from 'NewsApi'." << std::endl;
                        overallSuccess = false;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error fetching news from source 'NewsApi': " << e.what() << std::endl;
                overallSuccess = false;
            } catch (...) {
                std::cerr << "Unknown error fetching news from source 'NewsApi'" << std::endl;
                overallSuccess = false;
            }
        }
    }
    
    {
        std::shared_ptr<INewsSource> theNewsApi = getNewsSource("TheNewsApi");
        if (theNewsApi && theNewsApi->isActive()) {
            try {
                std::cout << "Sequentially fetching news from source 'TheNewsApi'..." << std::endl;
                auto articles = theNewsApi->fetchNews();
                std::cout << "Fetched " << articles.size() << " articles from 'TheNewsApi'." << std::endl;
                
                if (auto api = std::dynamic_pointer_cast<TheNewsApi>(theNewsApi)) {
                    if (api->processAndStoreArticles(articles)) {
                        std::cout << "Successfully processed and stored articles from 'TheNewsApi'." << std::endl;
                    } else {
                        std::cerr << "Failed to process some articles from 'TheNewsApi'." << std::endl;
                        overallSuccess = false;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error fetching news from source 'TheNewsApi': " << e.what() << std::endl;
                overallSuccess = false;
            } catch (...) {
                std::cerr << "Unknown error fetching news from source 'TheNewsApi'" << std::endl;
                overallSuccess = false;
            }
        }
    }
    
    std::cout << "Sequential fetching complete." << std::endl;
    return overallSuccess;
}
