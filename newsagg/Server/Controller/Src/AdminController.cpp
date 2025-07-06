#include "../Inc/AdminController.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

ExternalServerService& AdminController::getExternalServerService() {
    static ExternalServerService service;
    return service;
}

CategoryService& AdminController::getCategoryService() {
    static CategoryService service;
    return service;
}

void AdminController::registerRoutes(HttpServer& server) {
    std::cout << "Registering admin routes..." << std::endl;
    
    server.get("/api/admin/external-servers", handleGetExternalServers);
    server.get("/api/admin/external-servers/:id", handleGetExternalServerById);
    server.put("/api/admin/external-servers/:id", handleUpdateExternalServer);
    
    server.post("/api/admin/categories", handleAddCategory);
    
    std::cout << "Admin routes registered." << std::endl;
}

void AdminController::handleGetExternalServers(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling get all external servers request" << std::endl;
    
    try {
        auto& service = getExternalServerService();
        auto servers = service.getAllExternalServers();
        
        nlohmann::json responseJson = nlohmann::json::array();
        for (const auto& server : servers) {
            responseJson.push_back(server->toJson());
        }
        
        nlohmann::json successResponse = {
            {"status", "success"},
            {"data", responseJson}
        };
        
        res.set_content(successResponse.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void AdminController::handleGetExternalServerById(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling get external server by id request" << std::endl;
    
    try {
        if (!req.has_param("id")) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Server ID parameter is required"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int serverId = std::stoi(req.path_params.at("id"));
        
        auto& service = getExternalServerService();
        auto server = service.getExternalServerById(serverId);
        
        if (!server) {
            res.status = 404;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "External server not found"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        nlohmann::json successResponse = {
            {"status", "success"},
            {"data", server->toJson()}
        };
        
        res.set_content(successResponse.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void AdminController::handleUpdateExternalServer(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling update external server request" << std::endl;
    
    try {
        if (req.path_params.find("id") == req.path_params.end()) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Server ID parameter is required in URL path"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        unsigned int serverId = std::stoi(req.path_params.at("id"));
        std::cout << "Updating server with ID: " << serverId << std::endl;
        
        nlohmann::json requestData = nlohmann::json::parse(req.body);
        std::cout << "Request body: " << requestData.dump() << std::endl;
        auto& service = getExternalServerService();
        
        auto existingServer = service.getExternalServerById(serverId);
        if (!existingServer) {
            res.status = 404;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "External server not found"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        if (requestData.contains("apiKey") && !requestData["apiKey"].is_null()) {
            existingServer->apiKey = requestData["apiKey"].get<std::string>();
        }
        
        if (requestData.contains("apiStatus") && !requestData["apiStatus"].is_null()) {
            std::string status = requestData["apiStatus"].get<std::string>();
            existingServer->apiStatus = (status == "active") ? ApiStatus::ACTIVE : ApiStatus::NOT_ACTIVE;
        }
        
        bool updated = service.updateExternalServer(*existingServer);
        if (!updated) {
            res.status = 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to update external server"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        nlohmann::json successResponse = {
            {"status", "success"},
            {"message", "External server updated successfully"},
            {"data", existingServer->toJson()}
        };
        
        res.set_content(successResponse.dump(), "application/json");
    } catch (const nlohmann::json::parse_error& e) {
        res.status = 400;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Invalid JSON format: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void AdminController::handleAddCategory(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Handling add category request" << std::endl;
    
    try {
        nlohmann::json requestData = nlohmann::json::parse(req.body);
        Category newCategory = Category::fromJson(requestData);
        
        if (!newCategory.isValid()) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid category data. Category name is required."}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        auto& service = getCategoryService();
        
        auto existingCategory = service.getCategoryByName(newCategory.categoryName);
        if (existingCategory) {
            res.status = 409;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Category with this name already exists."}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        bool created = service.createCategory(newCategory);
        if (!created) {
            res.status = 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Failed to create category"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        auto createdCategory = service.getCategoryByName(newCategory.categoryName);
        if (!createdCategory) {
            res.status = 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Category was created but couldn't be retrieved"}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        nlohmann::json successResponse = {
            {"status", "success"},
            {"message", "Category created successfully"},
            {"data", createdCategory->toJson()}
        };
        
        res.status = 201;
        res.set_content(successResponse.dump(), "application/json");
    } catch (const nlohmann::json::parse_error& e) {
        res.status = 400;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Invalid JSON format: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 500;
        nlohmann::json errorResponse = {
            {"status", "error"},
            {"message", "Server error: " + std::string(e.what())}
        };
        res.set_content(errorResponse.dump(), "application/json");
    }
}
