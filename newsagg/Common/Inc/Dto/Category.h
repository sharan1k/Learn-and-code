#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct Category {
    unsigned int categoryId;
    std::string categoryName;

    Category() : categoryId(0) {}

    Category(const std::string& _categoryName)
        : categoryId(0), 
          categoryName(_categoryName) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["categoryId"] = categoryId;
        jsonData["categoryName"] = categoryName;
        return jsonData;
    }

    static Category fromJson(const nlohmann::json& jsonData) {
        Category category;

        if (jsonData.contains("categoryId") && !jsonData["categoryId"].is_null())
            category.categoryId = jsonData["categoryId"].get<unsigned int>();
        
        if (jsonData.contains("categoryName") && !jsonData["categoryName"].is_null())
            category.categoryName = jsonData["categoryName"].get<std::string>();
        
        return category;
    }

    bool isValid() const {
        return !categoryName.empty();
    }
};
