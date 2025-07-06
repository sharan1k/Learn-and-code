#pragma once

#include "../Inc/DbConnection.h"
#include "../../../Common/Inc/Dto/SavedArticle.h"
#include "../../../Common/Inc/Dto/Article.h"
#include <memory>
#include <vector>
#include <string>

class SavedArticleDao {
public:
    SavedArticleDao();
    ~SavedArticleDao();
    
    bool saveArticle(const SavedArticle& savedArticle);
    bool isArticleSavedByUser(unsigned int userId, unsigned int articleId);
    std::vector<std::shared_ptr<Article>> getSavedArticlesByUser(unsigned int userId);
    bool updateSavedArticle(const SavedArticle& savedArticle);
    bool removeSavedArticle(unsigned int userId, unsigned int articleId);
};
