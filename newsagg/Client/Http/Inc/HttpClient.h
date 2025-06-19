#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "../../Common/Inc/httplib.h"
#include <string>
#include <functional>
#include <iostream>

class HttpClient {
public:
    HttpClient(const std::string& host, int port);
    ~HttpClient();

    bool get(const std::string& path, 
             std::function<void(const httplib::Result&)> callback);

    bool post(const std::string& path, 
              const std::string& jsonBody,
              std::function<void(const httplib::Result&)> callback);

    bool put(const std::string& path, 
             const std::string& jsonBody,
             std::function<void(const httplib::Result&)> callback);

    bool del(const std::string& path,
             std::function<void(const httplib::Result&)> callback);

private:
    std::string m_host;
    int m_port;
    httplib::Client m_client;
};

#endif // HTTP_CLIENT_H
