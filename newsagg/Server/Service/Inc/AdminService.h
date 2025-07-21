#pragma once

#include "../../Dao/Inc/ReportedArticleDao.h"
#include "../../Dao/Inc/HiddenArticleDao.h"
#include "../../Dao/Inc/HiddenCategoryDao.h"
#include "../../../Common/Inc/Dto/Article.h"
#include <memory>
#include <vector>

class AdminService {
public:
    static AdminService& getInstance();
    
    bool reportArticle(unsigned int userId, unsigned int articleId);
    bool hideArticle(unsigned int articleId);
    bool unhideArticle(unsigned int articleId);
    bool hideCategory(unsigned int categoryId);
    bool unhideCategory(unsigned int categoryId);
    std::vector<std::shared_ptr<Article>> getUnseenReportedArticles();
    bool markReportAsSeen(unsigned int userId, unsigned int articleId);
    std::vector<unsigned int> getAllHiddenArticleIds();
    std::vector<unsigned int> getAllHiddenCategoryIds();
    bool isArticleHidden(unsigned int articleId);
    bool isCategoryHidden(unsigned int categoryId);
    
private:
    AdminService();
    
    ReportedArticleDao reportedArticleDao;
    HiddenArticleDao hiddenArticleDao;
    HiddenCategoryDao hiddenCategoryDao;
    
    static constexpr int AUTO_HIDE_THRESHOLD = 3;
};
