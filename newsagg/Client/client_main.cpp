#include "HttpClient.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main(int argc, char** argv) {
    std::cout << "NewsAgg Client Test Application" << std::endl;
    
    std::string host = "localhost";
    int port = 8080;
    
    if (argc >= 2) {
        host = argv[1];
    }
    if (argc >= 3) {
        try {
            port = std::stoi(argv[2]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid port number: " << argv[2] << std::endl;
            return 1;
        }
    }
    
    std::cout << "Connecting to server at " << host << ":" << port << std::endl;
    
   
    HttpClient client(host, port);
    
    std::cout << "Testing GET request to /api/test..." << std::endl;
    client.get("/api/test", [](const httplib::Result& result) {
        if (result) {
            std::cout << "Response status: " << result->status << std::endl;
            std::cout << "Response body: " << result->body << std::endl;
        } else {
            std::cout << "Request failed: " << result.error() << std::endl;
        }
    });
    
    std::cout << "Testing POST request to /api/test..." << std::endl;
    std::string jsonPayload = R"({"test":"Hello from client","timestamp":)" + 
                              std::to_string(std::time(nullptr)) + "}";
                              
    client.post("/api/test", jsonPayload, [](const httplib::Result& result) {
        if (result) {
            std::cout << "Response status: " << result->status << std::endl;
            std::cout << "Response body: " << result->body << std::endl;
        } else {
            std::cout << "Request failed: " << result.error() << std::endl;
        }
    });
    
    std::cout << "Tests completed." << std::endl;
    
    return 0;
}
