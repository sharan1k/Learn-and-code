#include "HttpServer.h"

HttpServer::HttpServer(int port) 
    : m_port(port),
      m_isRunning(false) {
    m_server.set_read_timeout(5, 0); 
    m_server.set_write_timeout(5, 0); 
    
    // Default error handler
    m_server.set_error_handler([](const auto& req, auto& res) {
        const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
        char buf[BUFSIZ];
        snprintf(buf, sizeof(buf), fmt, res.status);
        res.set_content(buf, "text/html");
    });
    
    // Default 404 handler
    m_server.set_exception_handler([](const auto& req, auto& res, std::exception_ptr ep) {
        res.status = 500;
        try {
            std::rethrow_exception(ep);
        } catch (std::exception& e) {
            res.set_content(e.what(), "text/plain");
        } catch (...) {
            res.set_content("Unknown Exception", "text/plain");
        }
    });
}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::get(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler) {
    m_server.Get(path, handler);
}

void HttpServer::post(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler) {
    m_server.Post(path, handler);
}

void HttpServer::put(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler) {
    m_server.Put(path, handler);
}

void HttpServer::del(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler) {
    m_server.Delete(path, handler);
}

bool HttpServer::start() {
    if (m_isRunning) return true;
    
    m_serverThread = std::make_unique<std::thread>([this]() {
        std::cout << "Starting HTTP server on port " << m_port << std::endl;
        m_isRunning = true;
        if (!m_server.listen("0.0.0.0", m_port)) {
            std::cerr << "Server failed to start on port " << m_port << std::endl;
            m_isRunning = false;
        }
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return m_isRunning;
}

void HttpServer::stop() {
    if (!m_isRunning) return; 
    
    m_server.stop();
    m_isRunning = false;
    
    if (m_serverThread && m_serverThread->joinable()) {
        m_serverThread->join();
    }
    
    std::cout << "HTTP server stopped" << std::endl;
}

bool HttpServer::isRunning() const {
    return m_isRunning;
}
