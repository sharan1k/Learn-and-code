#include "../Inc/AdminController.h"
#include <string>

ExternalServerService& AdminController::getExternalServerService() {
    static ExternalServerService service;
    return service;
}

CategoryService& AdminController::getCategoryService() {
    static CategoryService service;
    return service;
}

AdminService& AdminController::getAdminService() {
    return AdminService::getInstance();
}

void AdminController::sendSuccessResponse(httplib::Response& response, const nlohmann::json& data, int status, const std::string& message) {
    nlohmann::json response = {{"status", "success"}};
    
    if (!data.is_null()) {
        response["data"] = data;
    }
    
    if (!message.empty()) {
        response["message"] = message;
    }
    
    response.status = status;
    response.set_content(response.dump(), "application/json");
}

void AdminController::sendErrorResponse(httplib::Response& response, const std::string& message, int status) {
    nlohmann::json response = {
        {"status", "error"},
        {"message", message}
    };
    
    response.status = status;
    response.set_content(response.dump(), "application/json");
}

void AdminController::registerRoutes(HttpServer& server) {
    server.get("/api/admin/external-servers", handleGetExternalServers);
    server.get("/api/admin/external-servers/:id", handleGetExternalServerById);
    server.put("/api/admin/external-servers/:id", handleUpdateExternalServer);
    server.post("/api/admin/categories", handleAddCategory);
    server.get("/api/admin/reported-articles", handleGetReportedArticles);
    server.post("/api/admin/articles/:id/hide", handleHideArticle);
    server.post("/api/admin/articles/:id/unhide", handleUnhideArticle);
    server.post("/api/admin/categories/:id/hide", handleHideCategory);
    server.post("/api/admin/categories/:id/unhide", handleUnhideCategory);
    server.get("/api/admin/hidden-articles", handleGetHiddenArticles);
    server.get("/api/admin/hidden-categories", handleGetHiddenCategories);
}

void AdminController::handleGetExternalServers(const httplib::Request& request, httplib::Response& response) {
    try {
        auto& service = getExternalServerService();
        auto servers = service.getAllExternalServers();
        
        nlohmann::json responseJson = nlohmann::json::array();
        for (const auto& server : servers) {
            responseJson.push_back(server->toJson());
        }
        
        sendSuccessResponse(response, responseJson);
    } catch (const std::exception& exception) {
        sendErrorResponse(response, "Server error: " + std::string(exception.what()));
    }
}

void AdminController::handleGetExternalServerById(const httplib::Request& request, httplib::Response& response) {
    try {
        if (!request.has_param("id")) {
            sendErrorResponse(response, "Server ID parameter is required", 400);
            return;
        }
        
        unsigned int serverId = std::stoi(request.path_params.at("id"));
        
        auto& service = getExternalServerService();
        auto server = service.getExternalServerById(serverId);
        
        if (!server) {
            sendErrorResponse(response, "External server not found", 404);
            return;
        }
        
        sendSuccessResponse(response, server->toJson());
    } catch (const std::exception& exception) {
        sendErrorResponse(response, "Server error: " + std::string(exception.what()));
    }
}

void AdminController::handleUpdateExternalServer(const httplib::Request& request, httplib::Response& response) {
    try {
        if (request.path_params.find("id") == request.path_params.end()) {
            sendErrorResponse(response, "Server ID parameter is required in URL path", 400);
            return;
        }
        
        unsigned int serverId = std::stoi(request.path_params.at("id"));
        nlohmann::json requestData = nlohmann::json::parse(request.body);
        auto& service = getExternalServerService();
        
        auto existingServer = service.getExternalServerById(serverId);
        if (!existingServer) {
            sendErrorResponse(response, "External server not found", 404);
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
            sendErrorResponse(response, "Failed to update external server", 500);
            return;
        }
        
        sendSuccessResponse(response, existingServer->toJson(), 200, "External server updated successfully");
    } catch (const nlohmann::json::parse_error& exception) {
        sendErrorResponse(response, "Invalid JSON format: " + std::string(exception.what()), 400);
    } catch (const std::exception& exception) {
        sendErrorResponse(response, "Server error: " + std::string(exception.what()));
    }
}

void AdminController::handleAddCategory(const httplib::Request& request, httplib::Response& response) {
    try {
        nlohmann::json requestData = nlohmann::json::parse(request.body);
        Category newCategory = Category::fromJson(requestData);
        
        if (!newCategory.isValid()) {
            sendErrorResponse(response, "Invalid category data. Category name is required.", 400);
            return;
        }
        
        auto& service = getCategoryService();
        
        auto existingCategory = service.getCategoryByName(newCategory.categoryName);
        if (existingCategory) {
            sendErrorResponse(response, "Category with this name already exists.", 409);
            return;
        }
        
        bool created = service.createCategory(newCategory);
        if (!created) {
            sendErrorResponse(response, "Failed to create category", 500);
            return;
        }
        
        auto createdCategory = service.getCategoryByName(newCategory.categoryName);
        if (!createdCategory) {
            sendErrorResponse(response, "Category was created but couldn't be retrieved", 500);
            return;
        }
        
        sendSuccessResponse(response, createdCategory->toJson(), 201, "Category created successfully");
    } catch (const nlohmann::json::parse_error& exception) {
        sendErrorResponse(response, "Invalid JSON format: " + std::string(exception.what()), 400);
    } catch (const std::exception& exception) {
        sendErrorResponse(response, "Server error: " + std::string(exception.what()));
    }
}
