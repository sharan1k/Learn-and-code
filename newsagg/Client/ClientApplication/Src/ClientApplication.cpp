#include "../Inc/ClientApplication.h"
#include <iostream>
#include <limits>
#include <regex>
#include <thread>
#include <chrono>

ClientApplication::ClientApplication(const std::string& host, int port)
    : isLoggedIn(false), isRunning(false), currentUser(nullptr) {
    
    httpClient = std::make_shared<HttpClient>(host, port);
    userHandler = std::make_unique<UserHandler>(httpClient);
}

void ClientApplication::start() {
    isRunning = true;
    while (isRunning) {
        showMainMenu();
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
    
    userHandler->login(username, password, [this, &requestComplete](bool success, const std::string& message, const User* user) {
        if (success) {
            isLoggedIn = true;
            currentUser = const_cast<User*>(user); // Safe since UserHandler owns the user
            std::cout << "Login successful! Welcome, " << user->userName << "!" << std::endl;
        } else {
            std::cout << "Login failed: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) { // 5 second timeout
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
    
    userHandler->signup(username, email, password, [this, &requestComplete](bool success, const std::string& message, const User* user) {
        if (success) {
            std::cout << "Registration successful! " << message << std::endl;
            std::cout << "You can now log in with your credentials." << std::endl;
        } else {
            std::cout << "Registration failed: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) { // 5 second timeout
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
