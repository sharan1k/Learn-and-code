#include "../Inc/NewsSourceManager.h"
#include "../Inc/TheNewsApi.h"
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

bool NewsSourceManager::registerNewsSource(std::shared_ptr<INewsSource> newsSource, const std::string& apiKey) {
    if (!newsSource) {
        std::cerr << "Error: Cannot register null news source" << std::endl;
        return false;
    }
    
    const std::string& sourceName = newsSource->getName();
    
    {
        std::lock_guard<std::mutex> lock(sourcesMutex);
        auto it = newsSources.find(sourceName);
        if (it != newsSources.end()) {
            std::cout << "News source '" << sourceName << "' is already registered." << std::endl;
            return true;
        }
    }
    
    auto existingServer = serverDao.findByName(sourceName);
    
    if (!existingServer) {
        ExternalServer server;
        server.apiName = sourceName;
        server.apiStatus = ApiStatus::ACTIVE;
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        server.lastAccessed = ss.str();
        
        server.apiKey = apiKey;
        
        if (!serverDao.createExternalServer(server)) {
            std::cerr << "Failed to register news source '" << sourceName << "' in the database." << std::endl;
            return false;
        }
        
        std::cout << "News source '" << sourceName << "' registered in the database." << std::endl;
    } else if (!apiKey.empty() && existingServer->apiKey != apiKey) {
        existingServer->apiKey = apiKey;
        if (!serverDao.updateExternalServer(*existingServer)) {
            std::cerr << "Failed to update API key for news source '" << sourceName << "'." << std::endl;
            return false;
        }
    }
    
    std::string effectiveApiKey = apiKey;
    if (existingServer && apiKey.empty()) {
        effectiveApiKey = existingServer->apiKey;
    }
    
    if (!newsSource->initialize(effectiveApiKey)) {
        std::cerr << "Failed to initialize news source '" << sourceName << "'." << std::endl;
        return false;
    }
    
    if (existingServer) {
        newsSource->setActive(existingServer->apiStatus == ApiStatus::ACTIVE);
    }
    
    {
        std::lock_guard<std::mutex> lock(sourcesMutex);
        newsSources[sourceName] = newsSource;
    }
    
    std::cout << "News source '" << sourceName << "' registered and initialized." << std::endl;
    return true;
}

void NewsSourceManager::startFetchingNews(int intervalMinutes) {
    std::lock_guard<std::mutex> lock(sourcesMutex);
    
    for (auto& pair : newsSources) {
        auto& newsSource = pair.second;
        if (newsSource && newsSource->isActive()) {
            if (auto theNewsApi = std::dynamic_pointer_cast<TheNewsApi>(newsSource)) {
                theNewsApi->startAutoFetch(intervalMinutes);
                
                std::string timeMessage;
                if (intervalMinutes >= 60 && intervalMinutes % 60 == 0) {
                    int hours = intervalMinutes / 60;
                    timeMessage = std::to_string(hours) + "-hour" + (hours > 1 ? "s" : "");
                } else {
                    timeMessage = std::to_string(intervalMinutes) + "-minute" + (intervalMinutes > 1 ? "s" : "");
                }
                
                std::cout << "Started auto-fetch for news source '" << newsSource->getName() 
                          << "' with " << timeMessage << " interval." << std::endl;
            }
        }
    }
}

void NewsSourceManager::stopFetchingNews() {
    std::lock_guard<std::mutex> lock(sourcesMutex);
    
    for (auto& pair : newsSources) {
        auto& newsSource = pair.second;
        if (newsSource) {
            if (auto theNewsApi = std::dynamic_pointer_cast<TheNewsApi>(newsSource)) {
                theNewsApi->stopAutoFetch();
                std::cout << "Stopped auto-fetch for news source '" << newsSource->getName() << "'." << std::endl;
            }
        }
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

bool NewsSourceManager::addOrUpdateNewsSource(const std::string& name, const std::string& apiKey) {
    auto source = getNewsSource(name);
    if (source) {
        if (!source->initialize(apiKey)) {
            std::cerr << "Failed to update news source '" << name << "'." << std::endl;
            return false;
        }
  
        auto existingServer = serverDao.findByName(name);
        if (existingServer) {
            existingServer->apiKey = apiKey;
            if (!serverDao.updateExternalServer(*existingServer)) {
                std::cerr << "Failed to update API key for news source '" << name << "'." << std::endl;
                return false;
            }
        }
    } else {
        auto newSource = createNewsSource(name);
        if (!newSource) {
            std::cerr << "Failed to create news source '" << name << "'." << std::endl;
            return false;
        }
        
        if (!registerNewsSource(newSource, apiKey)) {
            std::cerr << "Failed to register news source '" << name << "'." << std::endl;
            return false;
        }
    }
    
    return true;
}

void NewsSourceManager::loadNewsSourcesFromDatabase() {
    auto servers = serverDao.getAllExternalServers();
    
    for (const auto& server : servers) {
        auto newsSource = createNewsSource(server->apiName);
        if (newsSource) {
            if (registerNewsSource(newsSource, server->apiKey)) {
                std::cout << "Loaded news source '" << server->apiName << "' from database." << std::endl;
            } else {
                std::cerr << "Failed to register news source '" << server->apiName << "'." << std::endl;
            }
        } else {
            std::cerr << "Unknown news source type: '" << server->apiName << "'." << std::endl;
        }
    }
}

bool NewsSourceManager::fetchNewsNow() {
    bool success = true;
    
    std::lock_guard<std::mutex> lock(sourcesMutex);
    
    for (auto& pair : newsSources) {
        auto& newsSource = pair.second;
        if (newsSource && newsSource->isActive()) {
            try {
                std::cout << "Fetching news from source '" << newsSource->getName() << "'..." << std::endl;
                
                auto articles = newsSource->fetchNews();
                std::cout << "Fetched " << articles.size() << " articles from '" 
                          << newsSource->getName() << "'." << std::endl;
                
                if (auto theNewsApi = std::dynamic_pointer_cast<TheNewsApi>(newsSource)) {
                }
            } catch (const std::exception& e) {
                std::cerr << "Error fetching news from source '" << newsSource->getName() 
                          << "': " << e.what() << std::endl;
                success = false;
            }
        }
    }
    
    return success;
}

std::shared_ptr<INewsSource> NewsSourceManager::createNewsSource(const std::string& name) {
    if (name == "TheNewsApi") {
        return std::make_shared<TheNewsApi>();
    }
    
    return nullptr;
}
