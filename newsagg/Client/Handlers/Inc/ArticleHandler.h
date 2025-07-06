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

class ArticleHandler {
public:
    ArticleHandler(std::shared_ptr<HttpClient> httpClient);
    
    using HeadlinesCallback = std::function<void(bool success, const std::string& message, const std::vector<Article*>& articles)>;
    using ArticleDetailsCallback = std::function<void(bool success, const std::string& message, const Article* article)>;
    using CategoriesCallback = std::function<void(bool success, const std::string& message, const std::vector<Category*>& categories)>;
    using SaveArticleCallback = std::function<void(bool success, const std::string& message)>;
    using SavedArticlesCallback = std::function<void(bool success, const std::string& message, const std::vector<Article*>& articles)>;
    
    using HttpResultCallback = std::function<void(const httplib::Result& result)>;
    
    void getTodayHeadlines(int limit, HeadlinesCallback callback);
    void getHeadlinesByDateRange(const std::string& startDate, const std::string& endDate, int limit, HeadlinesCallback callback);
    void getHeadlinesByCategory(unsigned int categoryId, int limit, HeadlinesCallback callback);
    void getHeadlinesByDateRangeAndCategory(const std::string& startDate, const std::string& endDate, unsigned int categoryId, int limit, HeadlinesCallback callback);
    void getArticleDetails(unsigned int articleId, ArticleDetailsCallback callback);
    void getCategories(CategoriesCallback callback);
    void saveArticle(unsigned int userId, unsigned int articleId, SaveArticleCallback callback);
    void removeSavedArticle(unsigned int userId, unsigned int articleId, SaveArticleCallback callback);
    void getSavedArticles(unsigned int userId, SavedArticlesCallback callback);
    void searchArticles(const std::string& query, int limit,
                    const std::string& startDate = "", const std::string& endDate = "",
                    const std::string& sortBy = "", bool isDescending = true,
                    HeadlinesCallback callback = nullptr);
    
    void likeArticle(unsigned int userId, unsigned int articleId, SaveArticleCallback callback);
    void dislikeArticle(unsigned int userId, unsigned int articleId, SaveArticleCallback callback);
    
private:
    std::shared_ptr<HttpClient> httpClient;
};

#endif // ARTICLE_HANDLER_H
