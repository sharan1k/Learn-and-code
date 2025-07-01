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
    
    bool createArticle(const Article& article);
    std::shared_ptr<Article> findById(unsigned int articleId);
    std::vector<std::shared_ptr<Article>> findByCategory(unsigned int categoryId);
    std::vector<std::shared_ptr<Article>> getLatestArticles(int limit = 10);
    std::vector<std::shared_ptr<Article>> findBySource(const std::string& source);
    bool deleteArticle(unsigned int articleId);
    bool updateArticle(const Article& article);
    bool articleExists(const std::string& url);
};
