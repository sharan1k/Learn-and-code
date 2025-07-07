#include <iostream>
#include "ClientApplication.h"
#include "ArticleHandler.h"

void ClientApplication::showHeadlinesMenu() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to the News Application, " << currentUser->userName << "! Date: " << getCurrentDateString() << std::endl;
    std::cout << "Time: " << getCurrentTimeString() << std::endl;
    std::cout << "Please choose the options below" << std::endl;
    std::cout << "1. Today" << std::endl;
    std::cout << "2. Date range" << std::endl;
    std::cout << "3. Back to main menu" << std::endl;
    std::cout << "4. Logout" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    std::string choice = getInput("Enter your choice (1-4): ");
    
    if (choice == "1") {
        showCategoriesMenu();
    } else if (choice == "2") {
        showHeadlinesByDateMenu();
    } else if (choice == "3") {
        return;
    } else if (choice == "4") {
        handleLogout();
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
        waitForKeypress();
        showHeadlinesMenu();
    }
}

void ClientApplication::showHeadlinesByDateMenu() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Enter date range (YYYY-MM-DD format)" << std::endl;
    
    std::string startDate;
    std::string endDate;
    
    do {
        startDate = getInput("Start date (YYYY-MM-DD): ");
        if (!validateDateFormat(startDate)) {
            std::cout << "Invalid date format. Please use YYYY-MM-DD format." << std::endl;
        }
    } while (!validateDateFormat(startDate));
    
    do {
        endDate = getInput("End date (YYYY-MM-DD): ");
        if (!validateDateFormat(endDate)) {
            std::cout << "Invalid date format. Please use YYYY-MM-DD format." << std::endl;
        }
    } while (!validateDateFormat(endDate));
    
    showCategoriesMenu(startDate, endDate);
}

void ClientApplication::showCategoriesMenu(const std::string& startDate, const std::string& endDate) {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to the News Application, " << currentUser->userName << "! Date: " << getCurrentDateString() << std::endl;
    std::cout << "Time: " << getCurrentTimeString() << std::endl;
    std::cout << "Please choose the options below for Headlines" << std::endl;
    std::cout << "1. All" << std::endl;
    std::cout << "2. Business" << std::endl;
    std::cout << "3. Entertainment" << std::endl;
    std::cout << "4. Sports" << std::endl;
    std::cout << "5. Technology" << std::endl;
    std::cout << "6. Back" << std::endl;
    std::cout << "7. Logout" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    std::string choice = getInput("Enter your choice (1-7): ");
    bool requestComplete = false;
    
    auto processArticles = [this, &requestComplete](bool success, const std::string& message, const std::vector<Article*>& articles) {
        if (success) {
            displayHeadlines(articles);
        } else {
            std::cout << "Failed to fetch headlines: " << message << std::endl;
            waitForKeypress();
        }
        requestComplete = true;
    };
    
    if (choice == "1") {
        if (startDate.empty() && endDate.empty()) {
            articleHandler->getTodayHeadlines(10, processArticles);
        } else {
            articleHandler->getHeadlinesByDateRange(startDate, endDate, 10, processArticles);
        }
    } else if (choice == "2" || choice == "3" || choice == "4" || choice == "5") {
        unsigned int categoryId = 0;
        switch (choice[0]) {
            case '2': categoryId = 2; break;
            case '3': categoryId = 3; break; 
            case '4': categoryId = 6; break;
            case '5': categoryId = 7; break; 
        }
        
        if (startDate.empty() && endDate.empty()) {
            articleHandler->getHeadlinesByCategory(categoryId, 10, processArticles);
        } else {
            articleHandler->getHeadlinesByDateRangeAndCategory(startDate, endDate, categoryId, 10, processArticles);
        }
    } else if (choice == "6") {
        if (startDate.empty() && endDate.empty()) {
            showHeadlinesMenu();
        } else {
            showHeadlinesByDateMenu();
        }
        return;
    } else if (choice == "7") {
        handleLogout();
        return;
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
        waitForKeypress();
        showCategoriesMenu(startDate, endDate);
        return;
    }
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout++;
    }
    
    if (!requestComplete) {
        std::cout << "Request timed out. Please try again later." << std::endl;
        waitForKeypress();
    }
}

void ClientApplication::displayHeadlines(const std::vector<Article*>& articles) {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to the News Application, " << currentUser->userName << "! Date: " << getCurrentDateString() << " Time: " << getCurrentTimeString() << std::endl;
    std::cout << "H E A D L I N E S" << std::endl;
    
    if (articles.empty()) {
        std::cout << "No headlines found for the selected criteria." << std::endl;
    } else {
        int startIdx = 0;
        bool viewingHeadlines = true;
        
        while (viewingHeadlines) {
            int endIdx = std::min(startIdx + 10, static_cast<int>(articles.size()));
            
            for (int i = startIdx; i < endIdx; ++i) {
                const Article* article = articles[i];
                std::cout << "\n" << (i + 1) << ". Article Id: " << article->articleId << std::endl;
                std::cout << "Title: " << article->title << std::endl;
                std::cout << "URL: " << article->url << std::endl;
                std::cout << "Category: " << getCategoryNameById(article->categoryId) << std::endl;
            }
            
            std::cout << "\n=====================================" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "1. Save Article" << std::endl;
            std::cout << "2. Like Article" << std::endl;
            std::cout << "3. Dislike Article" << std::endl;
            
            if (endIdx < static_cast<int>(articles.size())) {
                std::cout << "4. Next Page" << std::endl;
            }
            
            if (startIdx > 0) {
                std::cout << "5. Previous Page" << std::endl;
            }
            
            std::cout << "6. Back" << std::endl;
            std::cout << "7. Logout" << std::endl;
            std::cout << "=====================================" << std::endl;
            
            std::string choice = getInput("Enter your choice: ");
            
            if (choice == "1") {
                unsigned int articleId = getIntInput("Enter article ID to save: ");
                saveArticle(articleId);
            } else if (choice == "2") {
                unsigned int articleId = getIntInput("Enter article ID to like: ");
                likeArticle(articleId);
            } else if (choice == "3") {
                unsigned int articleId = getIntInput("Enter article ID to dislike: ");
                dislikeArticle(articleId);
            } else if (choice == "4" && endIdx < static_cast<int>(articles.size())) {
                startIdx = endIdx;
            } else if (choice == "5" && startIdx > 0) {
                startIdx = std::max(0, startIdx - 10);
            } else if (choice == "6") {
                viewingHeadlines = false;
            } else if (choice == "7") {
                handleLogout();
                viewingHeadlines = false;
            } else {
                std::cout << "Invalid choice. Please try again." << std::endl;
                waitForKeypress();
            }
        }
    }
    
    for (Article* article : articles) {
        delete article;
    }
}

void ClientApplication::handleArticleDetails(unsigned int articleId) {
    std::cout << "Fetching article details..." << std::endl;
    
    bool requestComplete = false;
    articleHandler->getArticleDetails(articleId, [this, &requestComplete, articleId](bool success, const std::string& message, const Article* article) {
        if (success && article) {
            std::cout << "\n=====================================" << std::endl;
            std::cout << "Article ID: " << article->articleId << std::endl;
            std::cout << "Title: " << article->title << std::endl;
            std::cout << "Description: " << article->description << std::endl;
            std::cout << "Source: " << article->source << std::endl;
            std::cout << "URL: " << article->url << std::endl;
            std::cout << "Category: " << getCategoryNameById(article->categoryId) << std::endl;
            std::cout << "Published: " << article->publishedAt << std::endl;
            std::cout << "\n=====================================" << std::endl;
            
            std::cout << "Options:" << std::endl;
            std::cout << "1. Save Article" << std::endl;
            std::cout << "2. Like Article" << std::endl;
            std::cout << "3. Dislike Article" << std::endl;
            std::cout << "4. Back" << std::endl;
            std::cout << "5. Logout" << std::endl;
            
            std::string choice = getInput("Enter your choice (1-5): ");
            
            if (choice == "1") {
                saveArticle(articleId);
            } else if (choice == "2") {
                likeArticle(articleId);
            } else if (choice == "3") {
                dislikeArticle(articleId);
            } else if (choice == "4") {
            } else if (choice == "5") {
                handleLogout();
            } else {
                std::cout << "Invalid choice." << std::endl;
            }
            
            delete article;
        } else {
            std::cout << "Failed to fetch article details: " << message << std::endl;
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

void ClientApplication::saveArticle(unsigned int articleId) {
    std::cout << "Saving article..." << std::endl;
    
    if (!currentUser) {
        std::cout << "You need to be logged in to save articles." << std::endl;
        waitForKeypress();
        return;
    }
    
    bool requestComplete = false;
    articleHandler->saveArticle(currentUser->userId, articleId, [this, &requestComplete](bool success, const std::string& message) {
        if (success) {
            std::cout << "Article saved successfully!" << std::endl;
        } else {
            std::cout << "Failed to save article: " << message << std::endl;
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

void ClientApplication::showSavedArticlesMenu() {
    std::cout << "Fetching saved articles..." << std::endl;
    
    if (!currentUser) {
        std::cout << "You need to be logged in to view saved articles." << std::endl;
        waitForKeypress();
        return;
    }
    
    bool requestComplete = false;
    articleHandler->getSavedArticles(currentUser->userId, [this, &requestComplete](bool success, const std::string& message, const std::vector<Article*>& articles) {
        if (success) {
            if (articles.empty()) {
                std::cout << "You don't have any saved articles yet." << std::endl;
            } else {
                displaySavedArticles(articles);
                requestComplete = true;
                return;
            }
        } else {
            std::cout << "Failed to fetch saved articles: " << message << std::endl;
        }
        
        for (Article* article : articles) {
            delete article;
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

void ClientApplication::handleSearchArticles() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to the News Application, " << currentUser->userName << "! " 
              << getCurrentDateString() << " " << getCurrentTimeString() << std::endl;
    std::string searchQuery = getInput("Enter search query: ");
    
    std::string startDate = "";
    std::string endDate = "";
    std::string useDateRange = getInput("Filter by date range? (y/n): ");
    
    if (useDateRange == "y" || useDateRange == "Y") {
        do {
            startDate = getInput("Start date (YYYY-MM-DD): ");
            if (!validateDateFormat(startDate)) {
                std::cout << "Invalid date format. Please use YYYY-MM-DD format." << std::endl;
            }
        } while (!validateDateFormat(startDate));
        
        do {
            endDate = getInput("End date (YYYY-MM-DD): ");
            if (!validateDateFormat(endDate)) {
                std::cout << "Invalid date format. Please use YYYY-MM-DD format." << std::endl;
            }
        } while (!validateDateFormat(endDate));
    } else {
        std::cout << "Skipping date filter." << std::endl;
    }
    
    std::cout << "Sort by:" << std::endl;
    std::cout << "1. Published date" << std::endl;
    std::cout << "2. Likes descending" << std::endl;
    std::cout << "3. Dislikes descending" << std::endl;
    
    std::string sortOption = getInput("Choose option: ");
    std::string sortBy = "date";
    bool isDescending = true;
    
    if (sortOption == "2") {
        sortBy = "likes";
        isDescending = true;
    } else if (sortOption == "3") {
        sortBy = "dislikes";
        isDescending = true;
    }
    
    std::cout << "\nSearch Results:" << std::endl;
    std::cout << "Searching for articles..." << std::endl;
    
    bool requestComplete = false;
    articleHandler->searchArticles(searchQuery, 10, startDate, endDate, sortBy, isDescending,
        [this, &requestComplete](bool success, const std::string& message, const std::vector<Article*>& articles) {
        if (success) {
            if (articles.empty()) {
                std::cout << "No articles found matching your search criteria." << std::endl;
            } else {
                std::cout << "\nFound " << articles.size() << " article(s) matching your search criteria:" << std::endl;
                displayHeadlines(articles);
            }
        } else {
            std::cout << "Failed to search articles: " << message << std::endl;
            
            for (Article* article : articles) {
                delete article;
            }
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

void ClientApplication::likeArticle(unsigned int articleId) {
    std::cout << "Liking article..." << std::endl;
    
    if (!currentUser) {
        std::cout << "You need to be logged in to like articles." << std::endl;
        waitForKeypress();
        return;
    }
    
    bool requestComplete = false;
    
    articleHandler->likeArticle(currentUser->userId, articleId, [this, &requestComplete, articleId](bool success, const std::string& message) {
        if (success) {
            std::cout << "Article " << articleId << " liked successfully!" << std::endl;
        } else {
            std::cout << "Failed to like article: " << message << std::endl;
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

void ClientApplication::dislikeArticle(unsigned int articleId) {
    std::cout << "Disliking article..." << std::endl;
    
    if (!currentUser) {
        std::cout << "You need to be logged in to dislike articles." << std::endl;
        waitForKeypress();
        return;
    }
    
    bool requestComplete = false;
    
    articleHandler->dislikeArticle(currentUser->userId, articleId, [this, &requestComplete, articleId](bool success, const std::string& message) {
        if (success) {
            std::cout << "Article " << articleId << " disliked successfully!" << std::endl;
        } else {
            std::cout << "Failed to dislike article: " << message << std::endl;
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

void ClientApplication::displaySavedArticles(const std::vector<Article*>& articles) {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to the News Application, " << currentUser->userName << "! Date: " << getCurrentDateString() << " Time: " << getCurrentTimeString() << std::endl;
    std::cout << "S A V E D   A R T I C L E S" << std::endl;
    
    if (articles.empty()) {
        std::cout << "You don't have any saved articles." << std::endl;
    } else {
        int startIdx = 0;
        bool viewingSavedArticles = true;
        
        while (viewingSavedArticles) {
            int endIdx = std::min(startIdx + 10, static_cast<int>(articles.size()));
            
            for (int i = startIdx; i < endIdx; ++i) {
                const Article* article = articles[i];
                std::cout << "\n" << (i + 1) << ". Article Id: " << article->articleId << std::endl;
                std::cout << "Title: " << article->title << std::endl;
                std::cout << "URL: " << article->url << std::endl;
                std::cout << "Category: " << getCategoryNameById(article->categoryId) << std::endl;
            }
            
            std::cout << "\n=====================================" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "1. Remove Saved Article" << std::endl;
            
            if (endIdx < static_cast<int>(articles.size())) {
                std::cout << "2. Next Page" << std::endl;
            }
            
            if (startIdx > 0) {
                std::cout << "3. Previous Page" << std::endl;
            }
            
            std::cout << "4. Back" << std::endl;
            std::cout << "5. Logout" << std::endl;
            std::cout << "=====================================" << std::endl;
            
            std::string choice = getInput("Enter your choice: ");
            
            if (choice == "1") {
                unsigned int articleId = getIntInput("Enter article ID to remove: ");
                removeSavedArticle(articleId);                
                viewingSavedArticles = false;
            } else if (choice == "2" && endIdx < static_cast<int>(articles.size())) {
                startIdx = endIdx;
            } else if (choice == "3" && startIdx > 0) {
                startIdx = std::max(0, startIdx - 10);
            } else if (choice == "4") {
                viewingSavedArticles = false;
            } else if (choice == "5") {
                handleLogout();
                viewingSavedArticles = false;
            } else {
                std::cout << "Invalid choice. Please try again." << std::endl;
                waitForKeypress();
            }
        }
    }
    
    for (Article* article : articles) {
        delete article;
    }
}

void ClientApplication::removeSavedArticle(unsigned int articleId) {
    std::cout << "Removing saved article..." << std::endl;
    
    if (!currentUser) {
        std::cout << "You need to be logged in to remove saved articles." << std::endl;
        waitForKeypress();
        return;
    }
    
    bool requestComplete = false;
    articleHandler->removeSavedArticle(currentUser->userId, articleId, [this, &requestComplete](bool success, const std::string& message) {
        if (success) {
            std::cout << "Article removed from saved articles successfully!" << std::endl;
        } else {
            std::cout << "Failed to remove article: " << message << std::endl;
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

void ClientApplication::loadCategories() {
    categoryMap[1] = "All";
    categoryMap[2] = "Business";
    categoryMap[3] = "Entertainment";
    categoryMap[4] = "Sports";
    categoryMap[5] = "Technology";
    
    bool requestComplete = false;
    
    articleHandler->getCategories([this, &requestComplete](bool success, const std::string& message, const std::vector<Category*>& categories) {
        if (success) {
            for (const auto& category : categories) {
                categoryMap[category->categoryId] = category->categoryName;
            }
        } else {
            std::cout << "Failed to load categories: " << message << std::endl;
        }
        
        for (Category* category : categories) {
            delete category;
        }
        
        requestComplete = true;
    });
    
    int timeout = 0;
    while (!requestComplete && timeout < 50) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout++;
    }
}

std::string ClientApplication::getCategoryNameById(unsigned int categoryId) {
    if (categoryMap.empty()) {
        loadCategories();
    }
    
    if (categoryMap.find(categoryId) != categoryMap.end()) {
        return categoryMap[categoryId];
    }
    
    return "Unknown";
}