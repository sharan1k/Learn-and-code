#include "../Inc/UserController.h"

UserService& UserController::getUserService() {
    static UserService service;
    return service;
}

void UserController::sendSuccessResponse(httplib::Response& res, const nlohmann::json& data, int status, const std::string& message) {
    nlohmann::json response = {{"status", "success"}};
    
    if (!data.is_null()) {
        response["data"] = data;
    }
    
    if (!message.empty()) {
        response["message"] = message;
    }
    
    if (data.contains("user")) {
        response["user"] = data["user"];
    }
    
    res.status = status;
    res.set_content(response.dump(), "application/json");
}

void UserController::sendErrorResponse(httplib::Response& res, const std::string& message, int status) {
    nlohmann::json response = {
        {"status", "error"},
        {"message", message}
    };
    
    res.status = status;
    res.set_content(response.dump(), "application/json");
}

void UserController::registerRoutes(HttpServer& server) {
    server.post("/api/users/signup", handleSignup);
    server.post("/api/users/login", handleLogin);
}

void UserController::handleSignup(const httplib::Request& req, httplib::Response& res) {
    try {
        nlohmann::json requestData = nlohmann::json::parse(req.body);
        User newUser = User::fromJson(requestData);
        
        if (!newUser.isValid()) {
            sendErrorResponse(res, "Invalid user data. User name, email and password are required.", 400);
            return;
        }
        
        std::shared_ptr<User> createdUser;
        auto [success, message] = getUserService().registerUser(newUser, createdUser);
        
        if (success && createdUser != nullptr) {
            nlohmann::json userData;
            userData["user"] = createdUser->toJson();
            sendSuccessResponse(res, userData, 201, message);
        } else {
            int statusCode = message == "Email already registered." || message == "Username already taken." ? 409 : 500;
            sendErrorResponse(res, message, statusCode);
        }
    } catch (const nlohmann::json::parse_error& e) {
        sendErrorResponse(res, "Invalid JSON format: " + std::string(e.what()), 400);
    } catch (const std::exception& e) {
        sendErrorResponse(res, "Server error: " + std::string(e.what()));
    }
}

void UserController::handleLogin(const httplib::Request& req, httplib::Response& res) {
    try {
        nlohmann::json requestData = nlohmann::json::parse(req.body);
        
        if (!requestData.contains("userName") || !requestData.contains("password")) {
            sendErrorResponse(res, "Username and password are required.", 400);
            return;
        }
        
        std::string userName = requestData["userName"].get<std::string>();
        std::string password = requestData["password"].get<std::string>();
        
        auto user = getUserService().authenticateUser(userName, password);
        
        if (user != nullptr) {
            nlohmann::json userData;
            userData["user"] = user->toJson();
            sendSuccessResponse(res, userData, 200, "Login successful.");
        } else {
            sendErrorResponse(res, "Invalid username or password.", 401);
        }
    } catch (const nlohmann::json::parse_error& e) {
        sendErrorResponse(res, "Invalid JSON format: " + std::string(e.what()), 400);
    } catch (const std::exception& e) {
        sendErrorResponse(res, "Server error: " + std::string(e.what()));
    }
}
