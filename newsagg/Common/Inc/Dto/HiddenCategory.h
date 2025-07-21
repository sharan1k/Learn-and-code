#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct HiddenCategory {
    unsigned int categoryId;

    HiddenCategory() : categoryId(0) {}

    HiddenCategory(unsigned int _categoryId)
        : categoryId(_categoryId) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["categoryId"] = categoryId;
        return jsonData;
    }

    static HiddenCategory fromJson(const nlohmann::json& jsonData) {
        HiddenCategory hiddenCategory;

        if (jsonData.contains("categoryId") && !jsonData["categoryId"].is_null())
            hiddenCategory.categoryId = jsonData["categoryId"].get<unsigned int>();
        
        return hiddenCategory;
    }
};
