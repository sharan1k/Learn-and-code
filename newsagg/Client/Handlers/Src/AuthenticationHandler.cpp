#include "../Inc/AuthenticationHandler.h"
#include <iostream>
#include <regex>

AuthenticationHandler::AuthenticationHandler(std::shared_ptr<HttpClient> httpClient)
    : client(httpClient) {
}

std::string AuthenticationHandler::extractErrorMessage(
    const httplib::Result& result, 
    const std::string& defaultMessage,
    const std::map<int, std::string>& statusCodes) {
    
    std::string errorMessage = defaultMessage;
    
    try {
        auto errorJson = nlohmann::json::parse(result->body);
        
        if (errorJson.contains("message") && errorJson["message"].is_string()) {
            errorMessage = errorJson["message"];
        }
        
        auto it = statusCodes.find(result->status);
        if (it != statusCodes.end()) {
            errorMessage = it->second;
        }
    } catch (...) {
        errorMessage = defaultMessage + " (status " + std::to_string(result->status) + ")";
    }
    
    return errorMessage;
}

nlohmann::json AuthenticationHandler::parseResponse(
    const httplib::Result& result, 
    bool& success, 
    std::string& errorMessage) {
    
    if (!result) {
        success = false;
        errorMessage = "Network error: " + std::to_string(static_cast<int>(result.error()));
        return nlohmann::json();
    }
    
    try {
        nlohmann::json response = nlohmann::json::parse(result->body);
        if (response.contains("status") && response["status"] == "success") {
            success = true;
            return response;
        } else {
            success = false;
            errorMessage = response.value("message", "Unknown error");
            return response;
        }
    } catch (const std::exception& e) {
        success = false;
        errorMessage = "Error parsing server response: " + std::string(e.what());
        return nlohmann::json();
    }
}

void AuthenticationHandler::login(
    const std::string& userName, 
    const std::string& password,
    AuthCallback callback) {
    
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
        
        if (result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {401, "Invalid username or password"},
                {403, "Account is locked. Please contact support."},
                {404, "User not found"}
            };
            
            std::string errorMsg = extractErrorMessage(result, "Login failed", statusMessages);
            callback(false, errorMsg, nullptr);
            return;
        }
        
        try {
            nlohmann::json responseJson = nlohmann::json::parse(result->body);
            
            if (responseJson.contains("user")) {
                currentUser = User::fromJson(responseJson["user"]);
                callback(true, responseJson.value("message", "Login successful"), &currentUser);
            } else {
                callback(true, responseJson.value("message", "Login successful"), nullptr);
            }
        } catch (const std::exception& e) {
            callback(false, "Error processing login response: " + std::string(e.what()), nullptr);
        }
    });
}

void AuthenticationHandler::signup(
    const std::string& userName,
    const std::string& email,
    const std::string& password,
    AuthCallback callback) {
    
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
        
        if (result->status != 201) {
            std::map<int, std::string> statusMessages = {
                {400, "Invalid user data provided"},
                {409, "Username or email already exists"},
                {422, "Password does not meet security requirements"}
            };
            
            std::string errorMsg = extractErrorMessage(result, "Registration failed", statusMessages);
            callback(false, errorMsg, nullptr);
            return;
        }
        
        try {
            nlohmann::json responseJson = nlohmann::json::parse(result->body);
            
            if (responseJson.contains("user")) {
                currentUser = User::fromJson(responseJson["user"]);
                callback(true, responseJson.value("message", "Registration successful"), &currentUser);
            } else {
                callback(true, responseJson.value("message", "Registration successful"), nullptr);
            }
        } catch (const std::exception& e) {
            callback(false, "Error processing registration response: " + std::string(e.what()), nullptr);
        }
    });
}
