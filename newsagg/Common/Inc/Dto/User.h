#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct User {
    unsigned int userId;
    std::string userName;
    std::string emailId;
    std::string password;
    std::string role;

    User() : userId(0), role("user") {}

    User(const std::string& name, const std::string& email, const std::string& pwd = "")
        : userId(0), userName(name), emailId(email), password(pwd), role("user") {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["userId"] = userId;
        jsonData["userName"] = userName;
        jsonData["emailId"] = emailId;
        jsonData["role"] = role;
        return jsonData;
    }

    static User fromJson(const nlohmann::json& jsonData) {
        User user;

        if (jsonData.contains("userId") && !jsonData["userId"].is_null())
            user.userId = jsonData["userId"].get<unsigned int>();
        
        if (jsonData.contains("userName") && !jsonData["userName"].is_null())
            user.userName = jsonData["userName"].get<std::string>();
        
        if (jsonData.contains("emailId") && !jsonData["emailId"].is_null())
            user.emailId = jsonData["emailId"].get<std::string>();
        
        if (jsonData.contains("password") && !jsonData["password"].is_null())
            user.password = jsonData["password"].get<std::string>();
        
        if (jsonData.contains("role") && !jsonData["role"].is_null())
            user.role = jsonData["role"].get<std::string>();
        
        return user;
    }

    bool isValid() const {
        return !userName.empty() && !emailId.empty() && !password.empty();
    }
};
