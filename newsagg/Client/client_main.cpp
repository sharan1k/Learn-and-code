#include "ClientApplication/Inc/ClientApplication.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::cout << "News Aggregator Client" << std::endl;
    
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
      try {
        ClientApplication app(host, port);
        app.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
