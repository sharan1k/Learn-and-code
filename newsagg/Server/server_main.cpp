#include "HttpServer.h"
#include <iostream>
#include <string>
#include <csignal>

HttpServer* g_server = nullptr;

void signalHandler(int signum) {
    std::cout << "Signal received (" << signum << "). Shutting down..." << std::endl;
    if (g_server) {
        g_server->stop();
    }
    exit(signum);
}

int main(int argc, char** argv) {
    std::cout << "NewsAgg Server Test Application" << std::endl;
    
    int port = 8080;

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
    
    server.get("/api/test", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "Received GET request at /api/test" << std::endl;
        
        std::string response = R"({
            "message": "Hello from server",
            "timestamp": )" + std::to_string(std::time(nullptr)) + R"(
        })";
        
        res.set_content(response, "application/json");
    });
    
    server.post("/api/test", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "Received POST request at /api/test" << std::endl;
        std::cout << "Request body: " << req.body << std::endl;
        
        std::string response = R"({
            "serverMessage": "Successfully received POST data",
            "timestamp": )" + std::to_string(std::time(nullptr)) + R"(,
            "receivedData": )" + req.body + R"(
        })";
        
        res.set_content(response, "application/json");
    });
    
    std::cout << "Starting server on port " << port << "..." << std::endl;
    if (!server.start()) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1;
    }
    
    std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;
    
    while (server.isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    std::cout << "Server stopped." << std::endl;
    return 0;
}
