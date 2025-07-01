#pragma once

#include "INewsSource.h"
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class ArticleDao;
class CategoryDao;

class TheNewsApi : public INewsSource {
public:
    TheNewsApi();
    ~TheNewsApi() override;
    
    std::vector<Article> fetchNews() override;
    bool initialize(const std::string& apiKey) override;
    std::string getName() const override;
    bool isActive() const override;
    void setActive(bool active) override;
    void startAutoFetch(int intervalMinutes = 180);
    void stopAutoFetch();
    
private:
    std::string convertIsoToMySqlDateTime(const std::string& isoDateTime);
    
private:
    std::string apiKey;
    std::string apiUrl;
    std::atomic<bool> active;
    std::thread fetchThread;
    std::atomic<bool> running;
    std::mutex mutex;
    std::condition_variable cv;
    std::unique_ptr<ArticleDao> articleDao;
    std::unique_ptr<CategoryDao> categoryDao;
    std::string buildApiUrl() const;
    void autoFetchTask(int intervalMinutes);
    bool processAndStoreArticles(const std::vector<Article>& articles);
    void updateLastAccessed();
};
