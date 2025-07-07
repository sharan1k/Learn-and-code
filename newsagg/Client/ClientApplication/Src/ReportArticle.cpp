#include "ClientApplication.h"
#include <iostream>

void ClientApplication::reportArticle(unsigned int articleId) {
    if (!currentUser) {
        std::cout << "You need to be logged in to report articles." << std::endl;
        waitForKeypress();
        return;
    }
    
    bool requestComplete = false;
    
    articleHandler->reportArticle(
        currentUser->userId, 
        articleId,
        [this, &requestComplete, articleId](bool success, const std::string& message) {
            if (success) {
                std::cout << "Article " << articleId << " reported successfully!" << std::endl;
            } else {
                std::cout << "Failed to report article: " << message << std::endl;
            }
            waitForKeypress();
            requestComplete = true;
        }
    );
    
    if (!waitForRequestCompletion(requestComplete)) {
        std::cout << "Request timed out. Please try again later." << std::endl;
        waitForKeypress();
    }
}
