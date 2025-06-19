#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "../../Common/Inc/httplib.h"
#include <string>
#include <functional>
#include <map>
#include <iostream>
#include <thread>
#include <memory>

class HttpServer {
public:
    HttpServer(int port);
    ~HttpServer();

    void get(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler);
    void post(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler);
    void put(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler);
    void del(const std::string& path, std::function<void(const httplib::Request&, httplib::Response&)> handler);
    bool start();
    void stop();
    bool isRunning() const;

private:
    int m_port;
    httplib::Server m_server;
    std::unique_ptr<std::thread> m_serverThread;
    bool m_isRunning;
};

#endif // HTTP_SERVER_H
