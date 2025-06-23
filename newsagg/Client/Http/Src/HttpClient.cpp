#include "HttpClient.h"

HttpClient::HttpClient(const std::string& host, int port) 
    : host(host), 
      port(port),
      client(host, port) {
    client.set_connection_timeout(5, 0); 
}

HttpClient::~HttpClient() {
}

bool HttpClient::get(const std::string& path, 
                    std::function<void(const httplib::Result&)> callback) {
    try {
        auto res = client.Get(path);
        if (callback) {
            callback(res);
        }
        return res && res->status == 200;
    } 
    catch (const std::exception& e) {
        std::cerr << "GET request failed: " << e.what() << std::endl;
        return false;
    }
}

bool HttpClient::post(const std::string& path, 
                     const std::string& jsonBody,
                     std::function<void(const httplib::Result&)> callback) {
    try {
        auto res = client.Post(path, jsonBody, "application/json");
        if (callback) {
            callback(res);
        }
        return res && res->status == 200;
    } 
    catch (const std::exception& e) {
        std::cerr << "POST request failed: " << e.what() << std::endl;
        return false;
    }
}

bool HttpClient::put(const std::string& path, 
                    const std::string& jsonBody,
                    std::function<void(const httplib::Result&)> callback) {
    try {
        auto res = client.Put(path, jsonBody, "application/json");
        if (callback) {
            callback(res);
        }
        return res && res->status == 200;
    } 
    catch (const std::exception& e) {
        std::cerr << "PUT request failed: " << e.what() << std::endl;
        return false;
    }
}

bool HttpClient::del(const std::string& path,
                    std::function<void(const httplib::Result&)> callback) {
    try {
        auto res = client.Delete(path);
        if (callback) {
            callback(res);
        }
        return res && res->status == 200;
    } 
    catch (const std::exception& e) {
        std::cerr << "DELETE request failed: " << e.what() << std::endl;
        return false;
    }
}
