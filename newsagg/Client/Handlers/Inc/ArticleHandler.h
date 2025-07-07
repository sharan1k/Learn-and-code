#ifndef ARTICLE_HANDLER_H
#define ARTICLE_HANDLER_H

#include "../../../Common/Inc/Dto/Article.h"
#include "../../../Common/Inc/Dto/Category.h"
#include "../../../Common/Inc/Dto/SavedArticle.h"
#include "../../../Common/Inc/Dto/Like.h"
#include "../../Http/Inc/HttpClient.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <map>

class ArticleHandler {
public:
    using HeadlinesCallback = std::function<void(bool success, const std::string& message, const std::vector<Article*>& articles)>;
    using ArticleDetailsCallback = std::function<void(bool success, const std::string& message, const Article* article)>;
    using CategoriesCallback = std::function<void(bool success, const std::string& message, const std::vector<Category*>& categories)>;
    using StatusCallback = std::function<void(bool success, const std::string& message)>;
    using SavedArticlesCallback = std::function<void(bool success, const std::string& message, const std::vector<Article*>& articles)>;
    
    explicit ArticleHandler(std::shared_ptr<HttpClient> httpClient);
    
    void getTodayHeadlines(int limit, HeadlinesCallback callback);
    void getHeadlinesByDateRange(const std::string& startDate, const std::string& endDate, int limit, HeadlinesCallback callback);
    void getHeadlinesByCategory(unsigned int categoryId, int limit, HeadlinesCallback callback);
    void getHeadlinesByDateRangeAndCategory(const std::string& startDate, const std::string& endDate, unsigned int categoryId, int limit, HeadlinesCallback callback);
    void searchArticles(const std::string& query, int limit,
                    const std::string& startDate = "", const std::string& endDate = "",
                    const std::string& sortBy = "", bool isDescending = true,
                    HeadlinesCallback callback = nullptr);

    void getArticleDetails(unsigned int articleId, ArticleDetailsCallback callback);
    void getCategories(CategoriesCallback callback);
    
    void saveArticle(unsigned int userId, unsigned int articleId, StatusCallback callback);
    void removeSavedArticle(unsigned int userId, unsigned int articleId, StatusCallback callback);
    void getSavedArticles(unsigned int userId, SavedArticlesCallback callback);
    void likeArticle(unsigned int userId, unsigned int articleId, StatusCallback callback);
    void dislikeArticle(unsigned int userId, unsigned int articleId, StatusCallback callback);
    void reportArticle(unsigned int userId, unsigned int articleId, StatusCallback callback);
    
private:
    std::shared_ptr<HttpClient> httpClient;
    
    std::string extractErrorMessage(
        const httplib::Result& result,
        const std::string& defaultMessage,
        const std::map<int, std::string>& statusCodes = {});
    
    nlohmann::json parseResponse(
        const httplib::Result& result,
        bool& success,
        std::string& errorMessage);
};

#endif // ARTICLE_HANDLER_H
