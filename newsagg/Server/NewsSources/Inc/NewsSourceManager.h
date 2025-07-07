#pragma once

#include "INewsSource.h"
#include "../../Dao/Inc/ExternalServerDao.h"
#include "../../Service/Inc/NotificationService.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

class NewsSourceManager {
public:
    static NewsSourceManager& getInstance();
    
    NewsSourceManager(const NewsSourceManager&) = delete;
    NewsSourceManager& operator=(const NewsSourceManager&) = delete;
    NewsSourceManager(NewsSourceManager&&) = delete;
    NewsSourceManager& operator=(NewsSourceManager&&) = delete;
    
    void startFetchingNews(int intervalMinutes = 180);
    void stopFetchingNews();
    std::shared_ptr<INewsSource> getNewsSource(const std::string& name);
    std::vector<std::shared_ptr<INewsSource>> getAllNewsSources();
    void loadNewsSourcesFromDatabase();
    bool fetchNewsNow();

private:
    NewsSourceManager();
    ~NewsSourceManager();
    
    std::unordered_map<std::string, std::shared_ptr<INewsSource>> newsSources;
    std::mutex sourcesMutex;
    std::atomic<bool> isFetching;
    std::thread fetchThread;
    ExternalServerDao serverDao;
    std::shared_ptr<INewsSource> createNewsSource(const std::string& name);
    bool fetchNewsSequentially();
    bool initializeNewsSource(std::shared_ptr<INewsSource> newsSource, const std::string& apiKey);
};
