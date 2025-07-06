#include "../Inc/AdminHandler.h"
#include <iostream>

AdminHandler::AdminHandler(std::shared_ptr<HttpClient> client) : client(client) {
}

void AdminHandler::getExternalServers(std::function<void(bool success, const std::string& message, const std::vector<ExternalServer>& servers)> callback) {
    client->get("/api/admin/external-servers", [callback](const httplib::Result& result) {
        if (!result) {
            callback(false, "Network error", {});
            return;
        }
        
        if (result->status != 200) {
            try {
                auto errorJson = nlohmann::json::parse(result->body);
                std::string errorMessage = "Failed to retrieve external servers";
                
                if (errorJson.contains("message") && errorJson["message"].is_string()) {
                    errorMessage = errorJson["message"];
                }
                
                callback(false, errorMessage, {});
            } catch (...) {
                callback(false, "Server error (status " + std::to_string(result->status) + ")", {});
            }
            return;
        }

        try {
            nlohmann::json response = nlohmann::json::parse(result->body);
            
            if (response["status"] != "success") {
                callback(false, response.value("message", "Unknown error"), {});
                return;
            }
            
            std::vector<ExternalServer> servers;
            for (const auto& serverJson : response["data"]) {
                servers.push_back(ExternalServer::fromJson(serverJson));
            }
            
            callback(true, "Successfully retrieved external servers", servers);
        } catch (const std::exception& e) {
            callback(false, "Error parsing server response: " + std::string(e.what()), {});
        }
    });
}

void AdminHandler::getExternalServerDetails(int serverId, std::function<void(bool success, const std::string& message, const ExternalServer& server)> callback) {
    client->get("/api/admin/external-servers/" + std::to_string(serverId), [callback](const httplib::Result& result) {
        if (!result) {
            callback(false, "Network error", {});
            return;
        }
        
        if (result->status != 200) {
            try {
                auto errorJson = nlohmann::json::parse(result->body);
                std::string errorMessage = "Failed to retrieve server details";
                
                if (errorJson.contains("message") && errorJson["message"].is_string()) {
                    errorMessage = errorJson["message"];
                }
                
                if (result->status == 404) {
                    errorMessage = "Server not found";
                }
                
                callback(false, errorMessage, {});
            } catch (...) {
                callback(false, "Server error (status " + std::to_string(result->status) + ")", {});
            }
            return;
        }

        try {
            nlohmann::json response = nlohmann::json::parse(result->body);
            
            if (response["status"] != "success") {
                callback(false, response.value("message", "Unknown error"), {});
                return;
            }
            
            ExternalServer server = ExternalServer::fromJson(response["data"]);
            callback(true, "Successfully retrieved server details", server);
        } catch (const std::exception& e) {
            callback(false, "Error parsing server response: " + std::string(e.what()), {});
        }
    });
}

void AdminHandler::updateExternalServer(const ExternalServer& server, std::function<void(bool success, const std::string& message)> callback) {
    nlohmann::json requestBody = server.toJson();
    
    client->put("/api/admin/external-servers/" + std::to_string(server.apiId), 
                requestBody.dump(),
                [callback](const httplib::Result& result) {
        if (!result) {
            callback(false, "Network error");
            return;
        }
        
        if (result->status != 200) {
            try {
                auto errorJson = nlohmann::json::parse(result->body);
                std::string errorMessage = "Failed to update server";
                
                if (errorJson.contains("message") && errorJson["message"].is_string()) {
                    errorMessage = errorJson["message"];
                }
                
                if (result->status == 404) {
                    errorMessage = "Server not found";
                } else if (result->status == 400) {
                    errorMessage = "Invalid server data";
                }
                
                callback(false, errorMessage);
            } catch (...) {
                callback(false, "Server error (status " + std::to_string(result->status) + ")");
            }
            return;
        }

        try {
            nlohmann::json response = nlohmann::json::parse(result->body);
            
            if (response["status"] != "success") {
                callback(false, response.value("message", "Unknown error"));
                return;
            }
            
            callback(true, "Successfully updated server");
        } catch (const std::exception& e) {
            callback(false, "Error parsing server response: " + std::string(e.what()));
        }
    });
}

void AdminHandler::addCategory(const std::string& categoryName, std::function<void(bool success, const std::string& message)> callback) {
    nlohmann::json requestBody;
    requestBody["categoryName"] = categoryName;
    
    client->post("/api/admin/categories", 
                 requestBody.dump(),
                 [callback, categoryName](const httplib::Result& result) {
        if (!result) {
            callback(false, "Network error");
            return;
        }
        
        if (result->status != 201) {
            try {
                auto errorJson = nlohmann::json::parse(result->body);
                std::string errorMessage = "Failed to create category";
                
                if (errorJson.contains("message") && errorJson["message"].is_string()) {
                    errorMessage = errorJson["message"];
                }
                
                if (result->status == 409) {
                    errorMessage = "Category '" + categoryName + "' already exists";
                } else if (result->status == 400) {
                    errorMessage = "Invalid category name";
                }
                
                callback(false, errorMessage);
            } catch (...) {
                callback(false, "Server error (status " + std::to_string(result->status) + ")");
            }
            return;
        }

        try {
            nlohmann::json response = nlohmann::json::parse(result->body);
            
            if (response["status"] != "success") {
                callback(false, response.value("message", "Unknown error"));
                return;
            }
            
            callback(true, "Category '" + categoryName + "' added successfully");
        } catch (const std::exception& e) {
            callback(false, "Error parsing server response: " + std::string(e.what()));
        }
    });
}
