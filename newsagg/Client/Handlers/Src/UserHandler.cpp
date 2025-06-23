#include "../Inc/UserHandler.h"
#include <iostream>
#include <regex>

UserHandler::UserHandler(std::shared_ptr<HttpClient> client)
    : client(client) {
}

void UserHandler::login(
    const std::string& userName, 
    const std::string& password,
    std::function<void(bool success, const std::string& message, const User* user)> callback) {
    
    nlohmann::json requestData = {
        {"userName", userName},
        {"password", password}
    };
      client->post("/api/users/login", requestData.dump(), [this, callback](const httplib::Result& result) {
        if (!result) {
            std::string errorMsg = "Connection error: " + std::to_string(static_cast<int>(result.error()));
            callback(false, errorMsg, nullptr);
            return;
        }
        
        try {
            nlohmann::json responseJson = nlohmann::json::parse(result->body);
            
            if (result->status == 200) {
                if (responseJson.contains("user")) {
                    currentUser = User::fromJson(responseJson["user"]);
                    callback(true, responseJson["message"].get<std::string>(), &currentUser);
                } else {
                    callback(true, responseJson["message"].get<std::string>(), nullptr);
                }
            } else {
                std::string errorMsg = "Unknown error";
                if (responseJson.contains("message")) {
                    errorMsg = responseJson["message"].get<std::string>();
                }
                callback(false, errorMsg, nullptr);
            }
        } catch (const std::exception& e) {
            callback(false, "Error parsing response: " + std::string(e.what()), nullptr);
        }
    });
}

void UserHandler::signup(
    const std::string& userName,
    const std::string& email,
    const std::string& password,
    std::function<void(bool success, const std::string& message, const User* user)> callback) {
    
    nlohmann::json requestData = {
        {"userName", userName},
        {"emailId", email},
        {"password", password}
    };
      client->post("/api/users/signup", requestData.dump(), [this, callback](const httplib::Result& result) {
        if (!result) {
            std::string errorMsg = "Connection error: " + std::to_string(static_cast<int>(result.error()));
            callback(false, errorMsg, nullptr);
            return;
        }
        
        try {
            nlohmann::json responseJson = nlohmann::json::parse(result->body);
            
            if (result->status == 201) {
                if (responseJson.contains("user")) {
                    currentUser = User::fromJson(responseJson["user"]);
                    callback(true, responseJson["message"].get<std::string>(), &currentUser);
                } else {
                    callback(true, responseJson["message"].get<std::string>(), nullptr);
                }
            } else {
                std::string errorMsg = "Unknown error";
                if (responseJson.contains("message")) {
                    errorMsg = responseJson["message"].get<std::string>();
                }
                callback(false, errorMsg, nullptr);
            }
        } catch (const std::exception& e) {
            callback(false, "Error parsing response: " + std::string(e.what()), nullptr);
        }
    });
}
