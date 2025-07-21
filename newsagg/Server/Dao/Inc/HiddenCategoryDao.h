#pragma once

#include "../Inc/DbConnection.h"
#include "../../../Common/Inc/Dto/HiddenCategory.h"
#include <memory>
#include <vector>
#include <string>

class HiddenCategoryDao {
public:
    HiddenCategoryDao();
    ~HiddenCategoryDao();
    
    bool hideCategory(unsigned int categoryId);
    bool unhideCategory(unsigned int categoryId);
    bool isCategoryHidden(unsigned int categoryId);
    std::vector<unsigned int> getAllHiddenCategoryIds();
};
