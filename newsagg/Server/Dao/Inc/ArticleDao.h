#pragma once

#include "../Inc/DbConnection.h"
#include "../../../Common/Inc/Dto/Article.h"
#include <memory>
#include <vector>
#include <string>

class ArticleDao {
public:
    ArticleDao();
    ~ArticleDao();
    
    bool createArticle(const Article& article, unsigned int* outArticleId = nullptr);
    std::shared_ptr<Article> findById(unsigned int articleId);
    std::vector<std::shared_ptr<Article>> findByCategory(unsigned int categoryId);
    std::vector<std::shared_ptr<Article>> getLatestArticles(int limit = 10);
    std::vector<std::shared_ptr<Article>> findBySource(const std::string& source);
    bool deleteArticle(unsigned int articleId);
    bool updateArticle(const Article& article);
    bool articleExists(const std::string& url);
    
    std::vector<std::shared_ptr<Article>> getArticlesByDate(const std::string& date, int limit = 10);
    std::vector<std::shared_ptr<Article>> getArticlesByDateRange(
        const std::string& startDate, const std::string& endDate, int limit = 10);
    std::vector<std::shared_ptr<Article>> getArticlesByDateAndCategory(
        const std::string& date, unsigned int categoryId, int limit = 10);
    std::vector<std::shared_ptr<Article>> getArticlesByDateRangeAndCategory(
        const std::string& startDate, const std::string& endDate, 
        unsigned int categoryId, int limit = 10);
        
    std::vector<std::shared_ptr<Article>> searchArticles(
        const std::string& query, int limit = 10);
    std::vector<std::shared_ptr<Article>> searchArticlesByDateRange(
        const std::string& query, const std::string& startDate, const std::string& endDate, 
        int limit = 10);
    std::vector<std::shared_ptr<Article>> searchArticlesSortedByLikes(
        const std::string& query, bool sortByLikes, bool isDescending = true, 
        int limit = 10);
    std::vector<std::shared_ptr<Article>> searchArticlesByDateRangeSortedByLikes(
        const std::string& query, const std::string& startDate, const std::string& endDate,
        bool sortByLikes, bool isDescending = true, int limit = 10);
};
