#include "../Inc/CategoryService.h"

CategoryService::CategoryService() {}

CategoryService::~CategoryService() {}

std::vector<std::shared_ptr<Category>> CategoryService::getAllCategories() {
    return categoryDao.getAllCategories();
}

std::shared_ptr<Category> CategoryService::getCategoryById(unsigned int categoryId) {
    return categoryDao.findById(categoryId);
}

std::shared_ptr<Category> CategoryService::getCategoryByName(const std::string& categoryName) {
    return categoryDao.findByName(categoryName);
}

bool CategoryService::createCategory(const Category& category) {
    return categoryDao.createCategory(category);
}
