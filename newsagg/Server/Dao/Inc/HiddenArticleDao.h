#pragma once

#include "../Inc/DbConnection.h"
#include "../../../Common/Inc/Dto/HiddenArticle.h"
#include <memory>
#include <vector>
#include <string>

class HiddenArticleDao {
public:
    HiddenArticleDao();
    ~HiddenArticleDao();
    
    bool hideArticle(unsigned int articleId);
    bool unhideArticle(unsigned int articleId);
    bool isArticleHidden(unsigned int articleId);
    std::vector<unsigned int> getAllHiddenArticleIds();
};
