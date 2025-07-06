#ifndef CLIENT_APPLICATION_H
#define CLIENT_APPLICATION_H

#include "../../Handlers/Inc/AuthenticationHandler.h"
#include "../../Handlers/Inc/AdminHandler.h"
#include "../../Http/Inc/HttpClient.h"
#include <memory>
#include <string>
#include <regex>
#include <vector>

class ClientApplication {
public:
    ClientApplication(const std::string& host, int port);
    void start();
    
private:
    void showMainMenu();
    void showUserMenu();
    void showAdminMenu();    
    void handleLogin();
    void handleSignup();
    void handleLogout();
    void listExternalServers();
    void viewExternalServerDetails();
    void updateExternalServer();
    void addNewsCategory();
    bool validateEmail(const std::string& email);
    bool validateUsername(const std::string& username);
    bool validatePassword(const std::string& password);
    std::string getInput(const std::string& prompt);
    std::string getPasswordInput(const std::string& prompt);
    int getIntInput(const std::string& prompt, int min = 0, int max = INT_MAX);
    void waitForKeypress(const std::string& prompt = "Press Enter to continue...");
    std::shared_ptr<HttpClient> httpClient;
    std::unique_ptr<AuthenticationHandler> authenticationHandler;
    std::unique_ptr<AdminHandler> adminHandler;
    User* currentUser;
    bool isLoggedIn;
    bool isRunning;
};

#endif // CLIENT_APPLICATION_H
