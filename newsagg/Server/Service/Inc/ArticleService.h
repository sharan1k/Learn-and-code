#pragma once

#include "../../Dao/Inc/ArticleDao.h"
#include "../../Dao/Inc/CategoryDao.h"
#include "../../Dao/Inc/SavedArticleDao.h"
#include "../../Dao/Inc/LikesDao.h"
#include "../../../Common/Inc/Dto/Article.h"
#include "../../../Common/Inc/Dto/Category.h"
#include "../../../Common/Inc/Dto/SavedArticle.h"
#include "../../../Common/Inc/Dto/Like.h"
#include <memory>
#include <vector>
#include <string>
#include <utility>

class ArticleService {
public:
    ArticleService();
    ~ArticleService();
    
    std::vector<std::shared_ptr<Article>> getTodayHeadlines(int limit = 10);
    std::vector<std::shared_ptr<Article>> getHeadlinesByDateRange(
        const std::string& startDate, const std::string& endDate, int limit = 10);
    std::vector<std::shared_ptr<Article>> getTodayHeadlinesByCategory(
        unsigned int categoryId, int limit = 10);
    std::vector<std::shared_ptr<Article>> getHeadlinesByDateRangeAndCategory(
        const std::string& startDate, const std::string& endDate, 
        unsigned int categoryId, int limit = 10);
    
    std::shared_ptr<Article> getArticleDetails(unsigned int articleId);
    
    bool saveArticle(unsigned int userId, unsigned int articleId);
    std::vector<std::shared_ptr<Article>> getSavedArticles(unsigned int userId);
    bool removeSavedArticle(unsigned int userId, unsigned int articleId);
    
    bool likeArticle(unsigned int userId, unsigned int articleId);
    bool dislikeArticle(unsigned int userId, unsigned int articleId);
    
    std::vector<std::shared_ptr<Article>> searchArticles(const std::string& query, int limit = 10);
    std::vector<std::shared_ptr<Article>> searchArticlesByDateRange(
        const std::string& query, const std::string& startDate, const std::string& endDate, 
        int limit = 10);
    std::vector<std::shared_ptr<Article>> searchArticlesSortedByLikes(
        const std::string& query, bool sortByLikes, bool isDescending = true, 
        int limit = 10);
    std::vector<std::shared_ptr<Article>> searchArticlesByDateRangeSortedByLikes(
        const std::string& query, const std::string& startDate, const std::string& endDate,
        bool sortByLikes, bool isDescending = true, int limit = 10);
    
    std::vector<std::shared_ptr<Category>> getAllCategories();
    
    static std::string getCurrentDate();

private:
    ArticleDao articleDao;
    CategoryDao categoryDao;
    SavedArticleDao savedArticleDao;
    LikesDao likesDao;
};
