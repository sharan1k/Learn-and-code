#include "../Inc/AdminService.h"
#include "../../Utils/Inc/Logger.h"

AdminService& AdminService::getInstance() {
    static AdminService instance;
    return instance;
}

AdminService::AdminService() {}

bool AdminService::reportArticle(unsigned int userId, unsigned int articleId) {
    bool success = reportedArticleDao.reportArticle(userId, articleId);
    
    if (success) {
        int reportCount = reportedArticleDao.getReportCountForArticle(articleId);
        Logger::info("Article " + std::to_string(articleId) + " has " + std::to_string(reportCount) + " reports");
        
        if (reportCount >= AUTO_HIDE_THRESHOLD) {
            Logger::info("Auto-hiding article " + std::to_string(articleId) + " due to report count reaching threshold (" + 
                      std::to_string(reportCount) + " >= " + std::to_string(AUTO_HIDE_THRESHOLD) + ")");
            hideArticle(articleId);
        }
    }
    
    return success;
}

bool AdminService::hideArticle(unsigned int articleId) {
    return hiddenArticleDao.hideArticle(articleId);
}

bool AdminService::unhideArticle(unsigned int articleId) {
    return hiddenArticleDao.unhideArticle(articleId);
}

bool AdminService::hideCategory(unsigned int categoryId) {
    return hiddenCategoryDao.hideCategory(categoryId);
}

bool AdminService::unhideCategory(unsigned int categoryId) {
    return hiddenCategoryDao.unhideCategory(categoryId);
}

std::vector<std::shared_ptr<Article>> AdminService::getUnseenReportedArticles() {
    return reportedArticleDao.getUnseenReportedArticles();
}

bool AdminService::markReportAsSeen(unsigned int userId, unsigned int articleId) {
    return reportedArticleDao.markReportAsSeen(userId, articleId);
}

std::vector<unsigned int> AdminService::getAllHiddenArticleIds() {
    return hiddenArticleDao.getAllHiddenArticleIds();
}

std::vector<unsigned int> AdminService::getAllHiddenCategoryIds() {
    return hiddenCategoryDao.getAllHiddenCategoryIds();
}

bool AdminService::isArticleHidden(unsigned int articleId) {
    return hiddenArticleDao.isArticleHidden(articleId);
}

bool AdminService::isCategoryHidden(unsigned int categoryId) {
    return hiddenCategoryDao.isCategoryHidden(categoryId);
}
