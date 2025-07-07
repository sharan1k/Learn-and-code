#include "../Inc/ClientApplication.h"
#include "../../Config.h"
#include <iostream>
#include <thread>
#include <chrono>

bool ClientApplication::waitForRequestCompletion(bool& requestComplete) {
    int timeout = 0;
    while (!requestComplete && timeout < ClientConfig::MAX_POLL_COUNT) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ClientConfig::POLLING_INTERVAL_MS));
        timeout++;
    }
    
    if (!requestComplete) {
        std::cout << ClientConfig::MSG_TIMEOUT << std::endl;
        return false;
    }
    
    return true;
}

void ClientApplication::displayMenuHeader(const std::string& title) {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to News Application, " << currentUser->userName << "! Date: " << getCurrentDateString() << std::endl;
    std::cout << "Time: " << getCurrentTimeString() << std::endl;
    std::cout << title << std::endl;
}

void ClientApplication::displayUnseenNotifications(const std::vector<Notification*>& notifications) {
    int count = 1;
    for (const auto& notification : notifications) {
        if (notification->seenStatus == ClientConfig::STATUS_UNSEEN) {
            std::cout << count << ". Article ID: " << notification->articleId << std::endl;
            count++;
            
            articleHandler->getArticleDetails(notification->articleId, [this](
                bool success, const std::string& message, const Article* article) { 
                
                if (success && article) {
                    std::cout << "   Title: " << article->title << std::endl;
                    std::cout << "   Published: " << article->publishedAt << std::endl; 
                    std::cout << "   Category: " << getCategoryNameById(article->categoryId) << std::endl;
                    std::cout << "   -------------------" << std::endl;
                } else {
                    std::cout << "   [Could not fetch article details: " << message << "]" << std::endl;
                    std::cout << "   -------------------" << std::endl;
                }
            });
        }
    }
}
