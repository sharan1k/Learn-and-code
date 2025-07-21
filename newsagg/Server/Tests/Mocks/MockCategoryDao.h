#pragma once

#include <gmock/gmock.h>
#include "../../Dao/Inc/CategoryDao.h"

class MockCategoryDao : public CategoryDao {
public:
    MockCategoryDao() : CategoryDao() {}
    ~MockCategoryDao() override = default;
    
    MOCK_METHOD(bool, createCategory, (const Category&));
    MOCK_METHOD(std::shared_ptr<Category>, findById, (unsigned int));
    MOCK_METHOD(std::shared_ptr<Category>, findByName, (const std::string&));
    MOCK_METHOD(std::vector<std::shared_ptr<Category>>, getAllCategories, ());
    MOCK_METHOD(bool, updateCategory, (const Category&));
    MOCK_METHOD(bool, deleteCategory, (unsigned int));
    MOCK_METHOD(unsigned int, findOrCreateCategory, (const std::string&));
};
