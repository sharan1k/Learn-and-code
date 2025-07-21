#pragma once

#include "../Inc/DbConnection.h"
#include "../../../Common/Inc/Dto/Category.h"
#include <memory>
#include <vector>
#include <string>

class CategoryDao {
public:
    CategoryDao();
    ~CategoryDao();
    
    bool createCategory(const Category& category);
    std::shared_ptr<Category> findById(unsigned int categoryId);
    std::shared_ptr<Category> findByName(const std::string& categoryName);
    std::vector<std::shared_ptr<Category>> getAllCategories();
    bool updateCategory(const Category& category);
    bool deleteCategory(unsigned int categoryId);
    unsigned int findOrCreateCategory(const std::string& categoryName);
};
