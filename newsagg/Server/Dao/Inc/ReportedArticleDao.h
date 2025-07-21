#pragma once

#include "../Inc/DbConnection.h"
#include "../../../Common/Inc/Dto/ReportedArticle.h"
#include "../../../Common/Inc/Dto/Article.h"
#include <memory>
#include <vector>
#include <string>

class ReportedArticleDao {
public:
    ReportedArticleDao();
    ~ReportedArticleDao();
    
    bool reportArticle(unsigned int userId, unsigned int articleId);
    bool markReportAsSeen(unsigned int userId, unsigned int articleId);
    std::vector<std::shared_ptr<Article>> getUnseenReportedArticles();
    int getReportCountForArticle(unsigned int articleId);
};
