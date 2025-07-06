#pragma once

#include "../../../Common/Inc/Dto/Like.h"
#include "../Inc/DbConnection.h"
#include <string>
#include <memory>
#include <vector>

class LikesDao {
public:
    LikesDao();
    ~LikesDao();

    bool addLike(unsigned int userId, unsigned int articleId, const std::string& likeDislike);
    bool hasUserLiked(unsigned int userId, unsigned int articleId);
};
