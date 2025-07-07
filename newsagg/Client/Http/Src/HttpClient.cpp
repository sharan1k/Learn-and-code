#include "HttpClient.h"
#include "../../Config.h"
#include <iostream>
#include <thread>
#include <chrono>

HttpClient::HttpClient(const std::string& host, int port) 
    : host(host), 
      port(port),
      client(host, port) {
    client.set_connection_timeout(ClientConfig::HTTP_CONNECTION_TIMEOUT, 0); 
    client.set_read_timeout(ClientConfig::HTTP_READ_TIMEOUT, 0);        
    client.set_write_timeout(ClientConfig::HTTP_WRITE_TIMEOUT, 0);       
    client.set_keep_alive(true);
}

HttpClient::~HttpClient() {
}

template<typename Func>
bool HttpClient::performRequest(const std::string& method, const std::string& path, Func requestFunc, 
                              std::function<void(const httplib::Result&)> callback) {
    int retries = 0;
    bool success = false;
    bool shouldRetry = true;
    
    while (retries < ClientConfig::HTTP_MAX_RETRIES && !success && shouldRetry) {
        try {
            std::cout << method << " request to " << path << " (attempt " << (retries + 1) << " of " 
                      << ClientConfig::HTTP_MAX_RETRIES << ")..." << std::endl;
            auto response = requestFunc();
            
            if (response) {
                shouldRetry = false;
                
                if (response->status >= 200 && response->status < 300) {
                    success = true;
                    std::cout << "Request returned status: " << response->status << std::endl;
                } else {
                    std::cout << "Request returned status: " << response->status << std::endl;
                    
                    if (response->status >= 400 && response->status < 500) {
                        if (callback) {
                            callback(response);
                        }
                        return false;
                    }
                }
                
                if (callback) {
                    callback(response);
                }
            } else {
                std::cout << "Request attempt " << (retries + 1) << " failed with error code: " << response.error() << std::endl;
                shouldRetry = true;
            }
            
            if (!success && shouldRetry && retries < ClientConfig::HTTP_MAX_RETRIES - 1) {
                int delayMs = 500 * (1 << retries);
                std::cout << "Retrying in " << (delayMs / 1000.0) << " seconds..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            }
            
            retries++;
            
        } catch (const std::exception& exception) {
            std::cerr << method << " request exception: " << exception.what() << std::endl;
            retries++;
            if (retries < ClientConfig::HTTP_MAX_RETRIES) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
    
    return success;
}

bool HttpClient::get(const std::string& path, 
                   std::function<void(const httplib::Result&)> callback) {
    return performRequest("GET", path, 
                        [this, &path]() { return client.Get(path); }, 
                        callback);
}

bool HttpClient::post(const std::string& path, 
                    const std::string& jsonBody,
                    std::function<void(const httplib::Result&)> callback) {
    return performRequest("POST", path, 
                        [this, &path, &jsonBody]() { return client.Post(path, jsonBody, "application/json"); }, 
                        callback);
}

bool HttpClient::put(const std::string& path, 
                   const std::string& jsonBody,
                   std::function<void(const httplib::Result&)> callback) {
    return performRequest("PUT", path, 
                        [this, &path, &jsonBody]() { return client.Put(path, jsonBody, "application/json"); }, 
                        callback);
}

bool HttpClient::del(const std::string& path,
                   std::function<void(const httplib::Result&)> callback) {
    return performRequest("DELETE", path, 
                        [this, &path]() { return client.Delete(path); }, 
                        callback);
}

std::string HttpClient::urlEncode(const std::string& value) {
    std::string result;
    result.reserve(value.size());
    
    for (auto& c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else if (c == ' ') {
            result += '+';
        } else {
            result += '%';
            char hex[3];
            sprintf(hex, "%02X", static_cast<unsigned char>(c));
            result += hex;
        }
    }
    
    return result;
}
