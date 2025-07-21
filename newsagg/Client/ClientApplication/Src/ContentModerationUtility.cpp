#include "ClientApplication.h"
#include <iostream>

void ClientApplication::hideArticle(unsigned int articleId) {
    bool requestComplete = false;
    
    adminHandler->hideArticle(articleId, [this, &requestComplete, articleId](bool success, const std::string& message) {
        if (success) {
            std::cout << "Article " << articleId << " hidden successfully!" << std::endl;
        } else {
            std::cout << "Failed to hide article: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    if (!waitForRequestCompletion(requestComplete)) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}

void ClientApplication::unhideArticle(unsigned int articleId) {
    bool requestComplete = false;
    
    adminHandler->unhideArticle(articleId, [this, &requestComplete, articleId](bool success, const std::string& message) {
        if (success) {
            std::cout << "Article " << articleId << " unhidden successfully!" << std::endl;
        } else {
            std::cout << "Failed to unhide article: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    if (!waitForRequestCompletion(requestComplete)) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}

void ClientApplication::hideCategory(unsigned int categoryId) {
    bool requestComplete = false;
    
    adminHandler->hideCategory(categoryId, [this, &requestComplete, categoryId](bool success, const std::string& message) {
        if (success) {
            std::cout << "Category " << categoryId << " hidden successfully!" << std::endl;
        } else {
            std::cout << "Failed to hide category: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    if (!waitForRequestCompletion(requestComplete)) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}

void ClientApplication::unhideCategory(unsigned int categoryId) {
    bool requestComplete = false;
    
    adminHandler->unhideCategory(categoryId, [this, &requestComplete, categoryId](bool success, const std::string& message) {
        if (success) {
            std::cout << "Category " << categoryId << " unhidden successfully!" << std::endl;
        } else {
            std::cout << "Failed to unhide category: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    if (!waitForRequestCompletion(requestComplete)) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}
