#include "../Inc/UserController.h"
#include <nlohmann/json.hpp>
#include <iostream>

UserService& UserController::getUserService() {
    static UserService service;
    return service;
}

void UserController::registerRoutes(HttpServer& server) {
    std::cout << "Registering user routes..." << std::endl;
    server.post("/api/users/signup", handleSignup);
    server.post("/api/users/login", handleLogin);
}

void UserController::handleSignup(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Received signup request" << std::endl;
    
    try {
        nlohmann::json requestData = nlohmann::json::parse(req.body);
        User newUser = User::fromJson(requestData);
        
        if (!newUser.isValid()) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid user data. User name, email and password are required."}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        std::shared_ptr<User> createdUser;
        auto [success, message] = getUserService().registerUser(newUser, createdUser);
        
        if (success && createdUser != nullptr) {
            res.status = 201;
            nlohmann::json successResponse = {
                {"status", "success"},
                {"message", message},
                {"user", createdUser->toJson()}
            };
            res.set_content(successResponse.dump(), "application/json");
        } else {
            res.status = message == "Email already registered." || message == "Username already taken." ? 409 : 500;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", message}
            };
            res.set_content(errorResponse.dump(), "application/json");
        }
        
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

void UserController::handleLogin(const httplib::Request& req, httplib::Response& res) {
    std::cout << "Received login request" << std::endl;
    
    try {
        nlohmann::json requestData = nlohmann::json::parse(req.body);
        
        if (!requestData.contains("userName") || !requestData.contains("password")) {
            res.status = 400;
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Username and password are required."}
            };
            res.set_content(errorResponse.dump(), "application/json");
            return;
        }
        
        std::string userName = requestData["userName"].get<std::string>();
        std::string password = requestData["password"].get<std::string>();
        
        auto user = getUserService().authenticateUser(userName, password);if (user != nullptr) {
            nlohmann::json successResponse = {
                {"status", "success"},
                {"message", "Login successful."},
                {"user", user->toJson()}
            };
            res.set_content(successResponse.dump(), "application/json");        } else {
            res.status = 401; 
            nlohmann::json errorResponse = {
                {"status", "error"},
                {"message", "Invalid username or password."}
            };
            res.set_content(errorResponse.dump(), "application/json");
        }
        
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
