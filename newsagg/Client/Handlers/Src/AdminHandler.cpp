#include "../Inc/AdminHandler.h"
#include <iostream>

AdminHandler::AdminHandler(std::shared_ptr<HttpClient> httpClient) : client(httpClient) {
}

std::string AdminHandler::extractErrorMessage(
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

nlohmann::json AdminHandler::parseResponse(
    const httplib::Result& result, 
    bool& success, 
    std::string& errorMessage) {
    
    if (!result) {
        success = false;
        errorMessage = "Network error";
        return nlohmann::json();
    }
    
    try {
        nlohmann::json response = nlohmann::json::parse(result->body);
        if (response["status"] == "success") {
            success = true;
            return response;
        } else {
            success = false;
            errorMessage = response.value("message", "Unknown error");
            return nlohmann::json();
        }
    } catch (const std::exception& exception) {
        success = false;
        errorMessage = "Error parsing server response: " + std::string(exception.what());
        return nlohmann::json();
    }
}

void AdminHandler::getExternalServers(ServerListCallback callback) {
    client->get("/api/admin/external-servers", [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to retrieve external servers"
            );
            callback(false, errorMessage, {});
            return;
        }

        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage, {});
            return;
        }
        
        try {
            std::vector<ExternalServer> servers;
            servers.reserve(response["data"].size());
            
            for (const auto& serverJson : response["data"]) {
                servers.push_back(ExternalServer::fromJson(serverJson));
            }
            
            callback(true, "Successfully retrieved external servers", servers);
        } catch (const std::exception& exception) {
            callback(false, "Error processing server data: " + std::string(exception.what()), {});
        }
    });
}

void AdminHandler::getExternalServerDetails(int serverId, ServerDetailsCallback callback) {
    const std::string endpoint = "/api/admin/external-servers/" + std::to_string(serverId);
    
    client->get(endpoint, [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {404, "Server not found"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to retrieve server details", 
                statusMessages
            );
            
            callback(false, errorMessage, {});
            return;
        }

        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage, {});
            return;
        }
        
        try {
            ExternalServer server = ExternalServer::fromJson(response["data"]);
            callback(true, "Successfully retrieved server details", server);
        } catch (const std::exception& exception) {
            callback(false, "Error processing server data: " + std::string(exception.what()), {});
        }
    });
}

void AdminHandler::updateExternalServer(const ExternalServer& server, StatusCallback callback) {
    const std::string endpoint = "/api/admin/external-servers/" + std::to_string(server.apiId);
    const nlohmann::json requestBody = server.toJson();
    
    client->put(endpoint, 
                requestBody.dump(),
                [this, callback](const httplib::Result& result) {
        if (result && result->status != 200) {
            std::map<int, std::string> statusMessages = {
                {404, "Server not found"},
                {400, "Invalid server data"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to update server", 
                statusMessages
            );
            
            callback(false, errorMessage);
            return;
        }

        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage);
            return;
        }
        
        callback(true, "Successfully updated server");
    });
}

void AdminHandler::addCategory(const std::string& categoryName, StatusCallback callback) {
    nlohmann::json requestBody;
    requestBody["categoryName"] = categoryName;
    
    client->post("/api/admin/categories", 
                 requestBody.dump(),
                 [this, callback, categoryName](const httplib::Result& result) {
        if (result && result->status != 201) {
            std::map<int, std::string> statusMessages = {
                {409, "Category '" + categoryName + "' already exists"},
                {400, "Invalid category name"}
            };
            
            std::string errorMessage = extractErrorMessage(
                result, 
                "Failed to create category", 
                statusMessages
            );
            
            callback(false, errorMessage);
            return;
        }

        bool success = false;
        std::string errorMessage;
        auto response = parseResponse(result, success, errorMessage);
        
        if (!success) {
            callback(false, errorMessage);
            return;
        }
        
        callback(true, "Category '" + categoryName + "' added successfully");
    });
}
