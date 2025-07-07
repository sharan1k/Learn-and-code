#include "HttpServer.h"
#include "Dao/Inc/DbConnection.h"
#include "Controller/Inc/UserController.h"
#include "Controller/Inc/AdminController.h"
#include "Controller/Inc/ArticleController.h"
#include "Controller/Inc/NotificationController.h"
#include "Utils/Inc/EmailService.h"
#include "Config.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <csignal>
#include <ctime>
#include <memory>

HttpServer* g_server = nullptr;

void signalHandler(int signum) {
    std::cout << "Signal received (" << signum << "). Shutting down..." << std::endl;
    
    if (g_server) {
        g_server->stop();
    }
    exit(signum);
}

void initDbConnection() {
    const std::string& dbHost = Config::DATABASE_HOST;
    const std::string& dbUser = Config::DATABASE_USER;
    const std::string& dbPassword = Config::DATABASE_PASSWORD;
    const std::string& dbSchema = Config::DATABASE_SCHEMA;
 
    std::cout << "Initializing database connection to " << dbHost << "..." << std::endl;
    try {
        DbConnection::initDbConnection(dbHost, dbUser, dbPassword, dbSchema);
        std::cout << "Database connection initialized successfully." << std::endl;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        std::cerr << "ErrorCode: " << e.getErrorCode() << std::endl;
        std::cerr << "Server will continue without database connectivity." << std::endl;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Server will continue without database connectivity." << std::endl;
    }
}

int main(int argc, char** argv) {
    std::cout << "NewsAgg Server Application" << std::endl;
    
    initDbConnection();
    
    int port = Config::SERVER_PORT;

    if (argc >= 2) {
        try {
            port = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid port number: " << argv[1] << std::endl;
            return 1;
        }
    }
    
    HttpServer server(port);
    g_server = &server;    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::cout << "Registering user controller routes..." << std::endl;
    UserController::registerRoutes(server);
    std::cout << "User controller routes registered." << std::endl;
    
    std::cout << "Registering admin controller routes..." << std::endl;
    AdminController::registerRoutes(server);
    std::cout << "Admin controller routes registered." << std::endl;
    
    std::cout << "Registering article controller routes..." << std::endl;
    ArticleController::registerRoutes(server);
    std::cout << "Article controller routes registered." << std::endl;
    
    std::cout << "Registering notification controller routes..." << std::endl;
    NotificationController::registerRoutes(server);
    std::cout << "Notification controller routes registered." << std::endl;
    
    EmailService::initializeEmailService();
    
    server.get("/api/health", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json healthStatus = {
            {"status", "ok"},
            {"serverTime", std::to_string(std::time(nullptr))}
        };
        res.set_content(healthStatus.dump(), "application/json");    
    });
    
    std::cout << "Starting server on port " << port << "..." << std::endl;
    
    server.printRegisteredPaths();
    
    if (!server.start()) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1;
    }
    
    std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;
    
    while (server.isServerRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    std::cout << "Server stopped." << std::endl;
    return 0;
}
