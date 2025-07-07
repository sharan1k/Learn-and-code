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
             
    std::string urlEncode(const std::string& value);

private:
    template<typename Func>
    bool performRequest(const std::string& method, const std::string& path, Func requestFunc, 
                      std::function<void(const httplib::Result&)> callback);

    std::string host;
    int port;
    httplib::Client client;
};

#endif // HTTP_CLIENT_H
