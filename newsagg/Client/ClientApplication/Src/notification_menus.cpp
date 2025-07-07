#include "../Inc/ClientApplication.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <algorithm>

void ClientApplication::showNotificationsMenu() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to News Application, " << currentUser->userName << "! Date: " << getCurrentDateString() << std::endl;
    std::cout << "Time: " << getCurrentTimeString() << std::endl;
    std::cout << "N O T I F I C A T I O N S" << std::endl;
    std::cout << "1. View Notifications" << std::endl;
    std::cout << "2. Configure Notifications" << std::endl;
    std::cout << "3. Back" << std::endl;
    std::cout << "4. Logout" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    std::string choice = getInput("Enter your choice (1-4): ");
    
    if (choice == "1") {
        viewNotifications();
    } else if (choice == "2") {
        configureNotifications();
    } else if (choice == "3") {
        return;
    } else if (choice == "4") {
        handleLogout();
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
        waitForKeypress();
        showNotificationsMenu();
    }
}

void ClientApplication::viewNotifications() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "V I E W  N O T I F I C A T I O N S" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    bool requestComplete = false;
    std::vector<Notification*> notifications;
    
    notificationHandler->getNotifications(currentUser->userId, [this, &notifications, &requestComplete](
        bool success, const std::string& message, const std::vector<Notification*>& fetchedNotifications) {
        
        if (success) {
            // Store all notifications but only process unseen ones
            notifications = fetchedNotifications;
            
            // Count unseen notifications
            int unseenCount = 0;
            for (const auto& notification : notifications) {
                if (notification->seenStatus == "unseen") {
                    unseenCount++;
                }
            }
            
            if (unseenCount == 0) {
                std::cout << "You have no new notifications." << std::endl;
            } else {
                std::cout << "You have " << unseenCount << " new notification(s):" << std::endl;
                
                int count = 1;
                for (const auto& notification : notifications) {
                    // Only display unseen notifications
                    if (notification->seenStatus == "unseen") {
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
                
                notificationHandler->markNotificationsAsSeen(currentUser->userId, [](bool success, const std::string& message) {
                    if (success) {
                        std::cout << "All notifications marked as seen." << std::endl;
                    } else {
                        std::cout << "Failed to mark notifications as seen: " << message << std::endl;
                    }
                });
            }
        } else {
            std::cout << "Failed to fetch notifications: " << message << std::endl;
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
    
    // Clean up notification objects
    for (auto notification : notifications) {
        delete notification;
    }
    
    waitForKeypress();
    showNotificationsMenu();
}

void ClientApplication::configureNotifications() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to the News Application, " << currentUser->userName << "! Date: " << getCurrentDateString() << std::endl;
    std::cout << "Time: " << getCurrentTimeString() << std::endl;
    std::cout << "C O N F I G U R E - N O T I F I C A T I O N S" << std::endl;
    std::cout << "1. Category Notifications" << std::endl;
    std::cout << "2. Keywords" << std::endl;
    std::cout << "3. Back" << std::endl;
    std::cout << "4. Logout" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    std::string choice = getInput("Enter your choice (1-4): ");
    
    if (choice == "1") {
        showNotificationCategoryMenu();
    } else if (choice == "2") {
        configureKeywords();
    } else if (choice == "3") {
        showNotificationsMenu();
    } else if (choice == "4") {
        handleLogout();
    } else {
        std::cout << "Invalid choice. Please try again." << std::endl;
        waitForKeypress();
        configureNotifications();
    }
}

void ClientApplication::showNotificationCategoryMenu() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "Welcome to the News Application, " << currentUser->userName << "! Date: " << getCurrentDateString() << std::endl;
    std::cout << "Time: " << getCurrentTimeString() << std::endl;
    std::cout << "C O N F I G U R E - N O T I F I C A T I O N S" << std::endl;
    
    loadCategories();
    
    if (categoryMap.empty()) {
        std::cout << "No categories available." << std::endl;
        waitForKeypress();
        configureNotifications();
        return;
    }
    
    bool requestComplete = false;
    std::vector<NotificationSetting*> userSettings;
    
    notificationHandler->getNotificationSettings(currentUser->userId, 
        [&userSettings, &requestComplete](bool success, const std::string& message, const std::vector<NotificationSetting*>& settings) {
            if (success) {
                userSettings = settings;
            } else {
                std::cout << "Failed to fetch notification settings: " << message << std::endl;
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
        configureNotifications();
        return;
    }
    
    std::map<unsigned int, bool> categoryEnabled;
    for (const auto& setting : userSettings) {
        categoryEnabled[setting->categoryId] = true;
    }
    
    int count = 1;
    std::vector<unsigned int> categoryIds;
    
    for (const auto& category : categoryMap) {
        bool enabled = categoryEnabled.find(category.first) != categoryEnabled.end();
        std::cout << count << ". " << category.second << " - " << (enabled ? "Enabled" : "Disabled") << std::endl;
        categoryIds.push_back(category.first);
        count++;
    }
    
    std::cout << count << ". Keywords" << std::endl;
    std::cout << (count + 1) << ". Back" << std::endl;
    std::cout << (count + 2) << ". Logout" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    int choice = getIntInput("Enter your option: ", 1, count + 2);
    
    if (choice <= categoryIds.size()) {
        unsigned int categoryId = categoryIds[choice - 1];
        std::string categoryName = categoryMap[categoryId];
        bool currentlyEnabled = categoryEnabled.find(categoryId) != categoryEnabled.end();
        
        std::cout << "Category: " << categoryName << " is currently " << (currentlyEnabled ? "Enabled" : "Disabled") << std::endl;
        std::cout << "1. " << (currentlyEnabled ? "Disable" : "Enable") << std::endl;
        std::cout << "2. Back" << std::endl;
        
        int toggleChoice = getIntInput("Enter your choice: ", 1, 2);
        
        if (toggleChoice == 1) {
            requestComplete = false;
            notificationHandler->updateNotificationSetting(currentUser->userId, categoryId, !currentlyEnabled, 
                [categoryName, &requestComplete](bool success, const std::string& message) {
                    if (success) {
                        std::cout << "Notification setting for " << categoryName << " updated successfully." << std::endl;
                    } else {
                        std::cout << "Failed to update notification setting: " << message << std::endl;
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
        
        showNotificationCategoryMenu();
    } else if (choice == count) {
        configureKeywords();
    } else if (choice == count + 1) {
        configureNotifications();
    } else if (choice == count + 2) {
        handleLogout();
    }
    
    for (auto setting : userSettings) {
        delete setting;
    }
}

void ClientApplication::configureKeywords() {
    std::cout << "\n=====================================" << std::endl;
    std::cout << "K E Y W O R D  N O T I F I C A T I O N S" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    bool requestComplete = false;
    std::vector<std::string> keywords;
    
    notificationHandler->getKeywords(currentUser->userId, [&keywords, &requestComplete](
        bool success, const std::string& message, const std::vector<std::string>& fetchedKeywords) {
            if (success) {
                keywords = fetchedKeywords;
                
                std::cout << "Your current notification keywords:" << std::endl;
                if (keywords.empty()) {
                    std::cout << "No keywords configured." << std::endl;
                } else {
                    for (size_t i = 0; i < keywords.size(); ++i) {
                        std::cout << (i + 1) << ". " << keywords[i] << std::endl;
                    }
                }
            } else {
                std::cout << "Failed to fetch keywords: " << message << std::endl;
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
        configureNotifications();
        return;
    }
    
    std::cout << "\nOptions:" << std::endl;
    std::cout << "1. Add a keyword" << std::endl;
    std::cout << "2. Remove a keyword" << std::endl;
    std::cout << "3. Back" << std::endl;
    
    int choice = getIntInput("Enter your choice: ", 1, 3);
    
    if (choice == 1) {
        std::string keyword = getInput("Enter a new keyword: ");
        
        if (keyword.empty()) {
            std::cout << "Keyword cannot be empty." << std::endl;
            waitForKeypress();
            configureKeywords();
            return;
        }
        
        if (std::find(keywords.begin(), keywords.end(), keyword) != keywords.end()) {
            std::cout << "This keyword already exists." << std::endl;
            waitForKeypress();
            configureKeywords();
            return;
        }
        
        requestComplete = false;
        notificationHandler->addKeyword(currentUser->userId, keyword, [&requestComplete](bool success, const std::string& message) {
            if (success) {
                std::cout << "Keyword added successfully." << std::endl;
            } else {
                std::cout << "Failed to add keyword: " << message << std::endl;
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
    } else if (choice == 2) {
        if (keywords.empty()) {
            std::cout << "No keywords to remove." << std::endl;
            waitForKeypress();
            configureKeywords();
            return;
        }
        
        int keywordIndex = getIntInput("Enter the number of the keyword to remove: ", 1, keywords.size());
        std::string keywordToRemove = keywords[keywordIndex - 1];
        
        requestComplete = false;
        notificationHandler->removeKeyword(currentUser->userId, keywordToRemove, [&requestComplete](bool success, const std::string& message) {
            if (success) {
                std::cout << "Keyword removed successfully." << std::endl;
            } else {
                std::cout << "Failed to remove keyword: " << message << std::endl;
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
    } else if (choice == 3) {
        waitForKeypress();
        configureNotifications();
        return;
    }
    
    waitForKeypress();
    configureKeywords();
}
