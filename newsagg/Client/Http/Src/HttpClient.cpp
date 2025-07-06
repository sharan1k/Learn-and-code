#include "HttpClient.h"
#include <iostream>
#include <thread>
#include <chrono>

HttpClient::HttpClient(const std::string& host, int port) 
    : host(host), 
      port(port),
      client(host, port) {
    client.set_connection_timeout(10, 0); 
    client.set_read_timeout(20, 0);        
    client.set_write_timeout(10, 0);       
    client.set_keep_alive(true);         
    
    std::cout << "HttpClient initialized with 20 second timeouts" << std::endl;
}

HttpClient::~HttpClient() {
}

bool HttpClient::get(const std::string& path, 
                    std::function<void(const httplib::Result&)> callback) {
    const int MAX_RETRIES = 3;
    int retries = 0;
    bool success = false;
    bool shouldRetry = true;
    
    while (retries < MAX_RETRIES && !success && shouldRetry) {
        try {
            std::cout << "GET request to " << path << " (attempt " << (retries + 1) << " of " << MAX_RETRIES << ")..." << std::endl;
            auto res = client.Get(path);
            
            if (res) {
                shouldRetry = false;
                
                if (res->status >= 200 && res->status < 300) {
                    success = true;
                    std::cout << "Request returned status: " << res->status << std::endl;
                } else {
                    std::cout << "Request returned status: " << res->status << std::endl;
                    
                    if (res->status == 400 || res->status == 404 || res->status == 409) {
                        if (callback) {
                            callback(res);
                        }
                        return false; 
                    }
                }
                
                if (callback) {
                    callback(res);
                }
            } else {
                std::cout << "Request attempt " << (retries + 1) << " failed with error code: " << res.error() << std::endl;                
                shouldRetry = true;
            }
            
            if (!success && shouldRetry && retries < MAX_RETRIES - 1) {
                int delayMs = 500 * (1 << retries);
                std::cout << "Retrying in " << (delayMs / 1000.0) << " seconds..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            }
            
            retries++;
            
        } catch (const std::exception& e) {
            std::cerr << "GET request exception: " << e.what() << std::endl;
            retries++;
            if (retries < MAX_RETRIES) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
    
    return success;
}

bool HttpClient::post(const std::string& path, 
                     const std::string& jsonBody,
                     std::function<void(const httplib::Result&)> callback) {
    const int MAX_RETRIES = 3;
    int retries = 0;
    bool success = false;
    bool shouldRetry = true;
    
    while (retries < MAX_RETRIES && !success && shouldRetry) {
        try {
            std::cout << "POST request to " << path << " (attempt " << (retries + 1) << " of " << MAX_RETRIES << ")..." << std::endl;
            auto res = client.Post(path, jsonBody, "application/json");
            
            if (res) {
                shouldRetry = false;
                
                if (res->status >= 200 && res->status < 300) {
                    success = true;
                    std::cout << "Request returned status: " << res->status << std::endl;
                } else {
                    std::cout << "Request returned status: " << res->status << std::endl;
                    
                    if (res->status == 409) {
                        if (callback) {
                            callback(res);
                        }
                        return false; 
                    }
                }
                
                if (callback) {
                    callback(res);
                }
            } else {
                std::cout << "Request attempt " << (retries + 1) << " failed with error code: " << res.error() << std::endl;
                shouldRetry = true;
            }
            
            if (!success && shouldRetry && retries < MAX_RETRIES - 1) {
                int delayMs = 500 * (1 << retries);
                std::cout << "Retrying in " << (delayMs / 1000.0) << " seconds..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            }
            
            retries++;
            
        } catch (const std::exception& e) {
            std::cerr << "POST request exception: " << e.what() << std::endl;
            retries++;
            if (retries < MAX_RETRIES) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
    
    return success;
}

bool HttpClient::put(const std::string& path, 
                    const std::string& jsonBody,
                    std::function<void(const httplib::Result&)> callback) {
    const int MAX_RETRIES = 3;
    int retries = 0;
    bool success = false;
    bool shouldRetry = true;
    
    while (retries < MAX_RETRIES && !success && shouldRetry) {
        try {
            std::cout << "PUT request to " << path << " (attempt " << (retries + 1) << " of " << MAX_RETRIES << ")..." << std::endl;
            auto res = client.Put(path, jsonBody, "application/json");
            
            if (res) {
                shouldRetry = false;
                
                if (res->status >= 200 && res->status < 300) {
                    success = true;
                    std::cout << "Request returned status: " << res->status << std::endl;
                } else {
                    std::cout << "Request returned status: " << res->status << std::endl;
                    
                    if (res->status == 400 || res->status == 404 || res->status == 409) {
                        if (callback) {
                            callback(res);
                        }
                        return false; 
                    }
                }
                
                if (callback) {
                    callback(res);
                }
            } else {
                std::cout << "Request attempt " << (retries + 1) << " failed with error code: " << res.error() << std::endl;                
                shouldRetry = true;
            }
            
            if (!success && shouldRetry && retries < MAX_RETRIES - 1) {
                int delayMs = 500 * (1 << retries);
                std::cout << "Retrying in " << (delayMs / 1000.0) << " seconds..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            }
            
            retries++;
            
        } catch (const std::exception& e) {
            std::cerr << "PUT request exception: " << e.what() << std::endl;
            retries++;
            if (retries < MAX_RETRIES) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
    
    return success;
}

bool HttpClient::del(const std::string& path,
                    std::function<void(const httplib::Result&)> callback) {
    const int MAX_RETRIES = 3;
    int retries = 0;
    bool success = false;
    bool shouldRetry = true;
    
    while (retries < MAX_RETRIES && !success && shouldRetry) {
        try {
            std::cout << "DELETE request to " << path << " (attempt " << (retries + 1) << " of " << MAX_RETRIES << ")..." << std::endl;
            auto res = client.Delete(path);
            
            if (res) {
                shouldRetry = false;
                
                if (res->status >= 200 && res->status < 300) {
                    success = true;
                    std::cout << "Request returned status: " << res->status << std::endl;
                } else {
                    std::cout << "Request returned status: " << res->status << std::endl;
                    
                    if (res->status == 400 || res->status == 404 || res->status == 409) {
                        if (callback) {
                            callback(res);
                        }
                        return false;
                    }
                }
                
                if (callback) {
                    callback(res);
                }
            } else {
                std::cout << "Request attempt " << (retries + 1) << " failed with error code: " << res.error() << std::endl;
                shouldRetry = true;
            }
            
            if (!success && shouldRetry && retries < MAX_RETRIES - 1) {
                int delayMs = 500 * (1 << retries);
                std::cout << "Retrying in " << (delayMs / 1000.0) << " seconds..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            }
            
            retries++;
            
        } catch (const std::exception& e) {
            std::cerr << "DELETE request exception: " << e.what() << std::endl;
            retries++;
            if (retries < MAX_RETRIES) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
    
    return success;
}
