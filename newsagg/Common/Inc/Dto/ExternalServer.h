#pragma once

#include <string>
#include <nlohmann/json.hpp>

enum class ApiStatus {
    ACTIVE,
    NOT_ACTIVE
};

struct ExternalServer {
    unsigned int apiId;
    std::string apiName;
    ApiStatus apiStatus;
    std::string lastAccessed;  // Using string for datetime to simplify JSON serialization
    std::string apiKey;

    ExternalServer() : apiId(0), apiStatus(ApiStatus::NOT_ACTIVE) {}

    ExternalServer(const std::string& _apiName, ApiStatus _apiStatus, 
                  const std::string& _lastAccessed, const std::string& _apiKey)
        : apiId(0), 
          apiName(_apiName), 
          apiStatus(_apiStatus), 
          lastAccessed(_lastAccessed), 
          apiKey(_apiKey) {}

    nlohmann::json toJson() const {
        nlohmann::json jsonData;
        jsonData["apiId"] = apiId;
        jsonData["apiName"] = apiName;
        jsonData["apiStatus"] = (apiStatus == ApiStatus::ACTIVE) ? "active" : "notActive";
        jsonData["lastAccessed"] = lastAccessed;
        jsonData["apiKey"] = apiKey;
        return jsonData;
    }

    static ExternalServer fromJson(const nlohmann::json& jsonData) {
        ExternalServer server;

        if (jsonData.contains("apiId") && !jsonData["apiId"].is_null())
            server.apiId = jsonData["apiId"].get<unsigned int>();
        
        if (jsonData.contains("apiName") && !jsonData["apiName"].is_null())
            server.apiName = jsonData["apiName"].get<std::string>();
        
        if (jsonData.contains("apiStatus") && !jsonData["apiStatus"].is_null()) {
            std::string status = jsonData["apiStatus"].get<std::string>();
            server.apiStatus = (status == "active") ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE;
        }
        
        if (jsonData.contains("lastAccessed") && !jsonData["lastAccessed"].is_null())
            server.lastAccessed = jsonData["lastAccessed"].get<std::string>();
        
        if (jsonData.contains("apiKey") && !jsonData["apiKey"].is_null())
            server.apiKey = jsonData["apiKey"].get<std::string>();
        
        return server;
    }

    bool isValid() const {
        return !apiName.empty() && !apiKey.empty();
    }
};
