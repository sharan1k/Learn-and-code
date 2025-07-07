#include "ClientApplication.h"
#include <iostream>

void ClientApplication::showContentModerationMenu() {
    bool viewingMenu = true;
    
    while (viewingMenu) {
        std::cout << "\n======= Content Moderation =======" << std::endl;
        std::cout << "1. View Reported Articles" << std::endl;
        std::cout << "2. View Hidden Articles" << std::endl;
        std::cout << "3. View Hidden Categories" << std::endl;
        std::cout << "4. Back to Admin Menu" << std::endl;
        std::cout << "5. Logout" << std::endl;
        std::cout << "=================================" << std::endl;
        
        std::string choice = getInput("Enter your choice (1-5): ");
        
        if (choice == "1") {
            viewReportedArticles();
        } else if (choice == "2") {
            viewHiddenArticles();
        } else if (choice == "3") {
            viewHiddenCategories();
        } else if (choice == "4") {
            viewingMenu = false;
        } else if (choice == "5") {
            handleLogout();
            viewingMenu = false;
        } else {
            std::cout << "Invalid choice. Please try again." << std::endl;
            waitForKeypress();
        }
    }
}

void ClientApplication::viewReportedArticles() {
    std::cout << "\n======= Reported Articles =======" << std::endl;
    
    bool requestComplete = false;
    
    adminHandler->getReportedArticles([this, &requestComplete](bool success, const std::string& message, const std::vector<Article*>& articles) {
        if (success) {
            if (articles.empty()) {
                std::cout << "No reported articles found." << std::endl;
            } else {
                std::cout << "Reported Articles:" << std::endl;
                int i = 1;
                for (const auto& article : articles) {
                    std::cout << i << ". Article ID: " << article->articleId 
                              << " - Title: " << article->title << std::endl;
                    i++;
                }
                
                bool takingActions = true;
                while (takingActions && !articles.empty()) {
                    std::cout << "\nActions:" << std::endl;
                    std::cout << "1. Hide an article" << std::endl;
                    std::cout << "2. Back to moderation menu" << std::endl;
                    
                    std::string actionChoice = getInput("Enter your choice (1-2): ");
                    
                    if (actionChoice == "1") {
                        unsigned int articleId = getIntInput("Enter Article ID to hide: ");
                        hideArticle(articleId);
                    } else if (actionChoice == "2") {
                        takingActions = false;
                    } else {
                        std::cout << "Invalid choice. Please try again." << std::endl;
                    }
                }
            }
            
            for (auto* article : articles) {
                delete article;
            }
        } else {
            std::cout << "Failed to retrieve reported articles: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    if (!waitForRequestCompletion(requestComplete)) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}

void ClientApplication::viewHiddenArticles() {
    std::cout << "\n======= Hidden Articles =======" << std::endl;
    
    bool requestComplete = false;
    
    adminHandler->getHiddenArticles([this, &requestComplete](bool success, const std::string& message, const std::vector<unsigned int>& articleIds) {
        if (success) {
            if (articleIds.empty()) {
                std::cout << "No hidden articles found." << std::endl;
                requestComplete = true;
                return;
            } 
            
            std::vector<unsigned int> fetchedArticleIds = articleIds; 
            std::map<unsigned int, std::string> articleTitles;
            int articlesRemaining = articleIds.size();
            
            for (const auto& articleId : articleIds) {
                articleHandler->getArticleDetails(articleId, 
                    [this, &requestComplete, &articleTitles, &articlesRemaining, &fetchedArticleIds](
                        bool detailSuccess, const std::string& detailMsg, const Article* article) {
                        
                        if (detailSuccess && article) {
                            articleTitles[article->articleId] = article->title;
                            delete article; 
                        }
                        
                        articlesRemaining--;
                        
                        if (articlesRemaining <= 0) {
                            std::cout << "Hidden Articles:" << std::endl;
                            for (size_t i = 0; i < fetchedArticleIds.size(); i++) {
                                unsigned int id = fetchedArticleIds[i];
                                std::string title = articleTitles.count(id) ? articleTitles[id] : "Title unavailable";
                                std::cout << (i + 1) << ". Article ID: " << id 
                                          << " - Title: " << title << std::endl;
                            }
                            
                            bool takingActions = true;
                            while (takingActions && !fetchedArticleIds.empty()) {
                                std::cout << "\nActions:" << std::endl;
                                std::cout << "1. Unhide an article" << std::endl;
                                std::cout << "2. Back to moderation menu" << std::endl;
                                
                                std::string actionChoice = getInput("Enter your choice (1-2): ");
                                
                                if (actionChoice == "1") {
                                    unsigned int articleId = getIntInput("Enter Article ID to unhide: ");
                                    unhideArticle(articleId);
                                } else if (actionChoice == "2") {
                                    takingActions = false;
                                } else {
                                    std::cout << "Invalid choice. Please try again." << std::endl;
                                }
                            }
                            
                            requestComplete = true;
                        }
                    });
            }
        } else {
            std::cout << "Failed to retrieve hidden articles: " << message << std::endl;
            requestComplete = true;
        }
    });
    
    if (!waitForRequestCompletion(requestComplete)) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}

void ClientApplication::viewHiddenCategories() {
    std::cout << "\n======= Hidden Categories =======" << std::endl;
    
    bool requestComplete = false;
    
    adminHandler->getHiddenCategories([this, &requestComplete](bool success, const std::string& message, const std::vector<unsigned int>& categoryIds) {
        if (success) {
            if (categoryIds.empty()) {
                std::cout << "No hidden categories found." << std::endl;
            } else {
                std::cout << "Hidden Category IDs:" << std::endl;
                for (size_t i = 0; i < categoryIds.size(); i++) {
                    std::cout << (i + 1) << ". Category ID: " << categoryIds[i] 
                              << " - Name: " << getCategoryNameById(categoryIds[i]) << std::endl;
                }
                
                bool takingActions = true;
                while (takingActions && !categoryIds.empty()) {
                    std::cout << "\nActions:" << std::endl;
                    std::cout << "1. Hide a category" << std::endl;
                    std::cout << "2. Unhide a category" << std::endl;
                    std::cout << "3. Back to moderation menu" << std::endl;
                    
                    std::string actionChoice = getInput("Enter your choice (1-3): ");
                    
                    if (actionChoice == "1") {
                        unsigned int categoryId = getIntInput("Enter Category ID to hide: ");
                        hideCategory(categoryId);
                    } else if (actionChoice == "2") {
                        unsigned int categoryId = getIntInput("Enter Category ID to unhide: ");
                        unhideCategory(categoryId);
                    } else if (actionChoice == "3") {
                        takingActions = false;
                    } else {
                        std::cout << "Invalid choice. Please try again." << std::endl;
                    }
                }
            }
        } else {
            std::cout << "Failed to retrieve hidden categories: " << message << std::endl;
        }
        requestComplete = true;
    });
    
    if (!waitForRequestCompletion(requestComplete)) {
        std::cout << "Request timed out. Please try again later." << std::endl;
    }
    
    waitForKeypress();
}
