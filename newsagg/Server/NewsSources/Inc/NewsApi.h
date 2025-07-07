#pragma once

#include "INewsSource.h"
#include <string>
#include <atomic>
#include <memory>

class ArticleDao;
class CategoryDao;
class NotificationService;

class NewsApi : public INewsSource {
public:
    NewsApi();
    ~NewsApi() override;
    
    std::vector<Article> fetchNews() override;
    bool initialize(const std::string& apiKey) override;
    std::string getName() const override;
    bool isActive() const override;
    void setActive(bool active) override;
    bool processAndStoreArticles(const std::vector<Article>& articles);
    
private:
    std::string apiKey;
    std::string apiUrl;
    std::atomic<bool> active;
    std::unique_ptr<ArticleDao> articleDao;
    std::unique_ptr<CategoryDao> categoryDao;
    void updateLastAccessed();
    std::string convertIsoToMySqlDateTime(const std::string& isoDateTime);
};
