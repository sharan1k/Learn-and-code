#include "HttpServer.h"
#include "../../Config.h"
#include <thread>

HttpServer::HttpServer(int port) 
    : port(port),
      isRunning(false) {
    server.set_read_timeout(Config::SERVER_READ_TIMEOUT, 0); 
    server.set_write_timeout(Config::SERVER_WRITE_TIMEOUT, 0);
    server.set_keep_alive_max_count(Config::SERVER_KEEPALIVE_MAX_COUNT);
    
    #ifdef CPPHTTPLIB_THREAD_POOL_COUNT
    int threadPoolSize = Config::SERVER_NUM_THREADS;
    std::cout << "Server using thread pool size: " << threadPoolSize << std::endl;
    #endif
    server.set_logger([](const httplib::Request &request, const httplib::Response &response) {
        std::cout << request.method << " " << request.path << " - Status: " << response.status << std::endl;
    });    server.set_error_handler([](const auto& request, auto& response) {
        if (response.body.empty()) {
            const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p><p>Path: %s</p>";
            char buf[BUFSIZ];
            snprintf(buf, sizeof(buf), fmt, response.status, request.path.c_str());
            response.set_content(buf, "text/html");
        }
    });
    
    server.set_exception_handler([](const auto& request, auto& response, std::exception_ptr ep) {
        response.status = 500;
        try {
            std::rethrow_exception(ep);
        } catch (std::exception& exception) {
            response.set_content(exception.what(), "text/plain");
        } catch (...) {
            response.set_content("Unknown Exception", "text/plain");
        }
    });
}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::get(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler) {
    server.Get(path, handler);
}

void HttpServer::post(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler) {
    std::cout << "Registering POST handler for path: " << path << std::endl;
    server.Post(path, handler);
}

void HttpServer::put(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler) {
    server.Put(path, handler);
}

void HttpServer::del(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler) {
    server.Delete(path, handler);
}

bool HttpServer::start() {
    if (isRunning) return true;
    
    serverThread = std::make_unique<std::thread>([this]() {
        std::cout << "Starting HTTP server on port " << port << std::endl;
        isRunning = true;
        if (!server.listen("0.0.0.0", port)) {
            std::cerr << "Server failed to start on port " << port << std::endl;
            isRunning = false;
        }
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return isRunning;
}

void HttpServer::stop() {
    if (!isRunning) return; 
    
    server.stop();
    isRunning = false;
    
    if (serverThread && serverThread->joinable()) {
        serverThread->join();
    }
    
    std::cout << "HTTP server stopped" << std::endl;
}

bool HttpServer::isServerRunning() const {
    return isRunning;
}
