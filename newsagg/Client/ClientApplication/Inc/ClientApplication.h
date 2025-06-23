#ifndef CLIENT_APPLICATION_H
#define CLIENT_APPLICATION_H

#include "../../Handlers/Inc/UserHandler.h"
#include "../../Http/Inc/HttpClient.h"
#include <memory>
#include <string>
#include <regex>

class ClientApplication {
public:
    ClientApplication(const std::string& host, int port);
    void start();
    
private:
    void showMainMenu();
    void handleLogin();
    void handleSignup();
    
    bool validateEmail(const std::string& email);
    bool validateUsername(const std::string& username);
    bool validatePassword(const std::string& password);
    
    std::string getInput(const std::string& prompt);
    std::string getPasswordInput(const std::string& prompt);
    
    std::shared_ptr<HttpClient> httpClient;
    std::unique_ptr<UserHandler> userHandler;
    User* currentUser;
    bool isLoggedIn;
    bool isRunning;
};

#endif // CLIENT_APPLICATION_H
