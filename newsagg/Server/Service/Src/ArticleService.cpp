#include "../Inc/ArticleService.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

ArticleService::ArticleService() {
}

ArticleService::~ArticleService() {
}

std::string ArticleService::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}

std::vector<std::shared_ptr<Article>> ArticleService::getTodayHeadlines(int limit) {
    std::string today = getCurrentDate();
    return articleDao.getArticlesByDate(today, limit);
}

std::vector<std::shared_ptr<Article>> ArticleService::getHeadlinesByDateRange(
    const std::string& startDate, const std::string& endDate, int limit) {
    return articleDao.getArticlesByDateRange(startDate, endDate, limit);
}

std::vector<std::shared_ptr<Article>> ArticleService::getTodayHeadlinesByCategory(
    unsigned int categoryId, int limit) {
    std::string today = getCurrentDate();
    return articleDao.getArticlesByDateAndCategory(today, categoryId, limit);
}

std::vector<std::shared_ptr<Article>> ArticleService::getHeadlinesByDateRangeAndCategory(
    const std::string& startDate, const std::string& endDate, 
    unsigned int categoryId, int limit) {
    return articleDao.getArticlesByDateRangeAndCategory(startDate, endDate, categoryId, limit);
}

std::shared_ptr<Article> ArticleService::getArticleDetails(unsigned int articleId) {
    return articleDao.findById(articleId);
}

bool ArticleService::saveArticle(unsigned int userId, unsigned int articleId) {
    auto article = articleDao.findById(articleId);
    if (!article) {
        return false;
    }
    
    SavedArticle savedArticle(userId, articleId);
    return savedArticleDao.saveArticle(savedArticle);
}

std::vector<std::shared_ptr<Article>> ArticleService::getSavedArticles(unsigned int userId) {
    return savedArticleDao.getSavedArticlesByUser(userId);
}

bool ArticleService::removeSavedArticle(unsigned int userId, unsigned int articleId) {
    return savedArticleDao.removeSavedArticle(userId, articleId);
}

std::vector<std::shared_ptr<Article>> ArticleService::searchArticles(const std::string& query, int limit) {
    return articleDao.searchArticles(query, limit);
}

std::vector<std::shared_ptr<Category>> ArticleService::getAllCategories() {
    return categoryDao.getAllCategories();
}

std::vector<std::shared_ptr<Article>> ArticleService::searchArticlesByDateRange(
    const std::string& query, const std::string& startDate, const std::string& endDate, int limit) {
    return articleDao.searchArticlesByDateRange(query, startDate, endDate, limit);
}

std::vector<std::shared_ptr<Article>> ArticleService::searchArticlesSortedByLikes(
    const std::string& query, bool sortByLikes, bool isDescending, int limit) {
    return articleDao.searchArticlesSortedByLikes(query, sortByLikes, isDescending, limit);
}

std::vector<std::shared_ptr<Article>> ArticleService::searchArticlesByDateRangeSortedByLikes(
    const std::string& query, const std::string& startDate, const std::string& endDate,
    bool sortByLikes, bool isDescending, int limit) {
    return articleDao.searchArticlesByDateRangeSortedByLikes(
        query, startDate, endDate, sortByLikes, isDescending, limit);
}

bool ArticleService::likeArticle(unsigned int userId, unsigned int articleId) {
    return likesDao.addLike(userId, articleId, "like");
}

bool ArticleService::dislikeArticle(unsigned int userId, unsigned int articleId) {
    return likesDao.addLike(userId, articleId, "dislike");
}




