#include "../Inc/ClientApplication.h"
#include <iostream>
#include <limits>
#include <regex>
#include <thread>
#include <chrono>

ClientApplication::ClientApplication(const std::string& host, int port)
    : isLoggedIn(false), isRunning(false), currentUser(nullptr) {
    
    httpClient = std::make_shared<HttpClient>(host, port);
    authenticationHandler = std::make_unique<AuthenticationHandler>(httpClient);
    adminHandler = std::make_unique<AdminHandler>(httpClient);
}

void ClientApplication::start() {
    isRunning = true;
    while (isRunning) {
        if (!isLoggedIn) {
            showMainMenu();
        } else if (currentUser && currentUser->role == "admin") {
            showAdminMenu();
        } else {
            showUserMenu();
        }
    }
}

void ClientApplication::showMainMenu() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to the News Aggregator application. Please choose the options below." << std::endl;
    std::cout << "1. Login" << std::endl;
    std::cout << "2. Sign up" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    std::string choice = getInput("Enter your choice (1-3): ");
    
    if (choice == "1") {
        handleLogin();
    } else if (choice == "2") {
        handleSignup();
    } else if (choice == "3") {
        std::cout << "Thank you for using News Aggregator. Goodbye!" << std::endl;
        isRunning = false;
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
    }
}

void ClientApplication::handleLogin() {
    std::cout << "\n====== Login ======" << std::endl;
    std::string username = getInput("Enter username: ");
    std::string password = getPasswordInput("Enter password: ");
    
    std::cout << "Logging in..." << std::endl;
    
    bool requestComplete = false;
    
    authenticationHandler->login(username, password, [this, &requestComplete](bool success, const std::string& message, const User* user) {
        if (success) {
            isLoggedIn = true;
            currentUser = const_cast<User*>(user); 
            std::cout << "Login successful! Welcome, " << user->userName << "!" << std::endl;
        } else {
            std::cout << "Login failed: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) { 
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout++;
    }
    
    if (!requestComplete) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
}

void ClientApplication::handleSignup() {
    std::cout << "\n====== Sign up ======" << std::endl;
    
    std::string username;
    do {
        username = getInput("Enter username: ");
        if (!validateUsername(username)) {
            std::cout << "Username must be at least 3 characters and contain only letters, numbers, and underscores." << std::endl;
        }
    } while (!validateUsername(username));
    
    std::string email;
    do {
        email = getInput("Enter email: ");
        if (!validateEmail(email)) {
            std::cout << "Please enter a valid email address." << std::endl;
        }
    } while (!validateEmail(email));
    
    std::string password;
    do {
        password = getPasswordInput("Enter password: ");
        if (!validatePassword(password)) {
            std::cout << "Password must be at least 8 characters and include uppercase, lowercase, and at least one number." << std::endl;
        }
    } while (!validatePassword(password));
      std::cout << "Creating account..." << std::endl;
    
    bool requestComplete = false;
    
    authenticationHandler->signup(username, email, password, [this, &requestComplete](bool success, const std::string& message, const User* user) {
        if (success) {
            std::cout << "Registration successful! " << message << std::endl;
            std::cout << "You can now log in with your credentials." << std::endl;
        } else {
            std::cout << "Registration failed: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout++;
    }
    
    if (!requestComplete) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
}

bool ClientApplication::validateEmail(const std::string& email) {
    const std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
}

bool ClientApplication::validateUsername(const std::string& username) {
    const std::regex pattern(R"([a-zA-Z0-9_]{3,})");
    return std::regex_match(username, pattern);
}

bool ClientApplication::validatePassword(const std::string& password) {
    if (password.length() < 8) return false;
    
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        if (std::islower(c)) hasLower = true;
        if (std::isdigit(c)) hasDigit = true;
    }
    
    return hasUpper && hasLower && hasDigit;
}

std::string ClientApplication::getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}

std::string ClientApplication::getPasswordInput(const std::string& prompt) {
    return getInput(prompt);
}

int ClientApplication::getIntInput(const std::string& prompt, int min, int max) {
    int value;
    while (true) {
        std::string input = getInput(prompt);
        try {
            value = std::stoi(input);
            if (value >= min && value <= max) {
                break;
            }
            std::cout << "Please enter a number between " << min << " and " << max << "." << std::endl;
        } catch (const std::exception&) {
            std::cout << "Invalid input. Please enter a valid number." << std::endl;
        }
    }
    return value;
}

void ClientApplication::waitForKeypress(const std::string& prompt) {
    std::cout << prompt;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void ClientApplication::showUserMenu() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to News Aggregator, " << currentUser->userName << "!" << std::endl;
    std::cout << "1. View News" << std::endl;
    std::cout << "2. Logout" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    std::string choice = getInput("Enter your choice (1-2): ");
    
    if (choice == "1") {
        std::cout << "News feature not implemented yet." << std::endl;
        waitForKeypress();
    } else if (choice == "2") {
        handleLogout();
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
        waitForKeypress();
    }
}

void ClientApplication::showAdminMenu() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to the News Aggregator application. Please choose the options below." << std::endl;
    std::cout << "1. View the list of external servers and status" << std::endl;
    std::cout << "2. View the external server's details" << std::endl;
    std::cout << "3. Update/Edit the external server's details" << std::endl;
    std::cout << "4. Add new News Category" << std::endl;
    std::cout << "5. Logout" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    std::string choice = getInput("Enter your choice (1-5): ");
    
    if (choice == "1") {
        listExternalServers();
    } else if (choice == "2") {
        viewExternalServerDetails();
    } else if (choice == "3") {
        updateExternalServer();
    } else if (choice == "4") {
        addNewsCategory();
    } else if (choice == "5") {
        handleLogout();
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
        waitForKeypress();
    }
}

void ClientApplication::handleLogout() {
    isLoggedIn = false;
    currentUser = nullptr;
    std::cout << "You have been logged out successfully." << std::endl;
    waitForKeypress();
}

void ClientApplication::listExternalServers() {
    std::cout << "\n====== List of external servers ======" << std::endl;
    
    bool requestComplete = false;
    
    adminHandler->getExternalServers([this, &requestComplete](bool success, const std::string& message, const std::vector<ExternalServer>& servers) {
        if (success) {
            std::cout << "\nList of external servers:" << std::endl;
            int i = 1;
            for (const auto& server : servers) {
                std::string status = (server.apiStatus == ApiStatus::ACTIVE) ? "Active" : "Not Active";
                std::cout << i << ". " << server.apiName << " - " << status << " - last accessed: " << server.lastAccessed << std::endl;
                i++;
            }
        } else {
            std::cout << "Failed to retrieve external servers: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout++;
    }
    
    if (!requestComplete) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}

void ClientApplication::viewExternalServerDetails() {
    std::cout << "\n====== External Server Details ======" << std::endl;
    
    bool requestComplete = false;
    std::vector<ExternalServer> serverList;
    
    adminHandler->getExternalServers([&serverList, &requestComplete](bool success, const std::string& message, const std::vector<ExternalServer>& servers) {
        if (success) {
            serverList = servers;
        } else {
            std::cout << "Failed to retrieve external servers: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout++;
    }
    
    if (!requestComplete) {
        std::cout << "Request timed out. Please try again later." << std::endl;
        waitForKeypress();
        return;
    }
    
    if (serverList.empty()) {
        std::cout << "No external servers available." << std::endl;
        waitForKeypress();
        return;
    }
    
    std::cout << "\nList of external server details:" << std::endl;
    for (size_t i = 0; i < serverList.size(); ++i) {
        std::cout << (i + 1) << ". " << serverList[i].apiName << " - API Key: " << serverList[i].apiKey << std::endl;
    }
    
    waitForKeypress();
}

void ClientApplication::updateExternalServer() {
    std::cout << "\n====== Update External Server ======" << std::endl;
    
    bool requestComplete = false;
    std::vector<ExternalServer> serverList;
    
    adminHandler->getExternalServers([&serverList, &requestComplete](bool success, const std::string& message, const std::vector<ExternalServer>& servers) {
        if (success) {
            serverList = servers;
        } else {
            std::cout << "Failed to retrieve external servers: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout++;
    }
    
    if (!requestComplete) {
        std::cout << "Request timed out. Please try again later." << std::endl;
        waitForKeypress();
        return;
    }
    
    if (serverList.empty()) {
        std::cout << "No external servers available." << std::endl;
        waitForKeypress();
        return;
    }
    
    std::cout << "\nList of external servers:" << std::endl;
    for (size_t i = 0; i < serverList.size(); ++i) {
        std::string statusStr = (serverList[i].apiStatus == ApiStatus::ACTIVE) ? "ACTIVE" : "INACTIVE";
        std::cout << (i + 1) << ". " << serverList[i].apiName << " (ID: " << serverList[i].apiId 
                  << ", Status: " << statusStr << ")" << std::endl;
    }
    
    int selection = getIntInput("Enter the external server number: ", 1, serverList.size()) - 1;
    ExternalServer selectedServer = serverList[selection];
    
    std::string maskedKey = selectedServer.apiKey;
    if (maskedKey.length() > 8) {
        maskedKey = maskedKey.substr(0, 4) + "****" + maskedKey.substr(maskedKey.length() - 4);
    }
    std::cout << "Current API Key: " << maskedKey << std::endl;
    
    std::string updatedApiKey = getInput("Enter the updated API key (or leave blank to keep current): ");
    
    if (updatedApiKey.empty()) {
        std::cout << "Using existing API key." << std::endl;
    } else {
        selectedServer.apiKey = updatedApiKey;
    }
    
    requestComplete = false;
    adminHandler->updateExternalServer(selectedServer, [&requestComplete](bool success, const std::string& message) {
        if (success) {
            std::cout << "External server updated successfully." << std::endl;
        } else {
            std::cout << "Failed to update external server: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    timeout = 0;
    while (!requestComplete && timeout < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout++;
    }
    
    if (!requestComplete) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}

void ClientApplication::addNewsCategory() {
    std::cout << "\n====== Add New Category ======" << std::endl;
    
    std::string categoryName = getInput("Enter the new category name: ");
    
    if (categoryName.empty()) {
        std::cout << "Category name cannot be empty." << std::endl;
        waitForKeypress();
        return;
    }
    
    bool requestComplete = false;
    adminHandler->addCategory(categoryName, [&requestComplete](bool success, const std::string& message) {
        if (success) {
            std::cout << "Category added successfully." << std::endl;
        } else {
            std::cout << "Failed to add category: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout++;
    }
    
    if (!requestComplete) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}
