#pragma once

#include "../../Dao/Inc/CategoryDao.h"
#include <memory>
#include <vector>
#include <string>

class CategoryService {
public:
    CategoryService();
    ~CategoryService();
    
    std::vector<std::shared_ptr<Category>> getAllCategories();
    std::shared_ptr<Category> getCategoryById(unsigned int categoryId);
    std::shared_ptr<Category> getCategoryByName(const std::string& categoryName);
    bool createCategory(const Category& category);
    
private:
    CategoryDao categoryDao;
};
