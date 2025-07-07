#include "HttpServer.h"
#include "Dao/Inc/DbConnection.h"
#include "Controller/Inc/UserController.h"
#include "Controller/Inc/AdminController.h"
#include "Controller/Inc/ArticleController.h"
#include "Controller/Inc/NotificationController.h"
#include "Utils/Inc/EmailService.h"
#include "Utils/Inc/Logger.h"
#include "Config.h"
#include <nlohmann/json.hpp>
#include <string>
#include <csignal>
#include <ctime>
#include <memory>

HttpServer* g_server = nullptr;

void signalHandler(int signum) {
    Logger::info("Signal received (" + std::to_string(signum) + "). Shutting down...");
    
    if (g_server) {
        g_server->stop();
    }
    exit(signum);
}

void initDbConnection() {
    const std::string& dbHost = Config::DATABASE_HOST;
    const std::string& dbUser = Config::DATABASE_USER;
    const std::string& dbSchema = Config::DATABASE_SCHEMA;
 
    Logger::info("Initializing database connection to " + dbHost + "...");
    try {
        DbConnection::initDbConnection(dbHost, dbUser, Config::DATABASE_PASSWORD, dbSchema);
        Logger::info("Database connection initialized successfully");
    } catch (sql::SQLException &e) {
        Logger::error("SQLException: " + std::string(e.what()));
        Logger::error("SQLState: " + std::string(e.getSQLState()));
        Logger::error("ErrorCode: " + std::to_string(e.getErrorCode()));
        Logger::warning("Server will continue without database connectivity");
    } catch (std::exception &e) {
        Logger::error("Error: " + std::string(e.what()));
        Logger::warning("Server will continue without database connectivity");
    }
}

int main(int argc, char** argv) {
    Logger::info("NewsAgg Server Application");
    Logger::info("=========================");
    
    initDbConnection();
    
    int port = Config::SERVER_PORT;

    if (argc >= 2) {
        try {
            port = std::stoi(argv[1]);
            Logger::info("Using custom port: " + std::to_string(port));
        } catch (const std::exception& e) {
            Logger::error("Invalid port number: " + std::string(argv[1]));
            Logger::error("Server will exit due to invalid port configuration");
            return 1;
        }
    } else {
        Logger::info("Using default port: " + std::to_string(port));
    }
    
    HttpServer server(port);
    g_server = &server;    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    Logger::info("Registering user controller routes...");
    UserController::registerRoutes(server);
    Logger::info("User controller routes registered");
    
    Logger::info("Registering admin controller routes...");
    AdminController::registerRoutes(server);
    Logger::info("Admin controller routes registered");
    
    Logger::info("Registering article controller routes...");
    ArticleController::registerRoutes(server);
    Logger::info("Article controller routes registered");
    
    Logger::info("Registering notification controller routes...");
    NotificationController::registerRoutes(server);
    Logger::info("Notification controller routes registered");
    
    Logger::info("Initializing email service...");
    EmailService::initializeEmailService();
    Logger::info("Email service initialized");
    
    Logger::info("Setting up health check endpoint...");
    server.get("/api/health", [](const httplib::Request& req, httplib::Response& res) {
        nlohmann::json healthStatus = {
            {"status", "ok"},
            {"serverTime", std::to_string(std::time(nullptr))}
        };
        res.set_content(healthStatus.dump(), "application/json");    
    });
    Logger::info("Health check endpoint configured");
    
    Logger::info("Starting server on port " + std::to_string(port) + "...");
    
    if (!server.start()) {
        Logger::error("Failed to start server on port " + std::to_string(port));
        return 1;
    }
    
    Logger::info("Server is running. Press Ctrl+C to stop");
    
    while (server.isServerRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    Logger::info("Server stopped");
    return 0;
}
