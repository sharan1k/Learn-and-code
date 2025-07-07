#include "../Inc/NotificationService.h"
#include "../../Utils/Inc/EmailService.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <cctype>
#include <set>

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

NotificationService::NotificationService() {
}

NotificationService::~NotificationService() {
}

std::vector<std::shared_ptr<Notification>> NotificationService::getNotifications(unsigned int userId) {
    try {
        return notificationDao.getNotificationsByUser(userId);
    } catch (const std::exception& e) {
        std::cerr << "Error in getNotifications: " << e.what() << std::endl;
        return {};
    }
}

bool NotificationService::markNotificationsAsSeen(unsigned int userId) {
    try {
        return notificationDao.markAllAsSeen(userId);
    } catch (const std::exception& e) {
        std::cerr << "Error in markNotificationsAsSeen: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<NotificationSetting>> NotificationService::getNotificationSettings(unsigned int userId) {
    try {
        return settingDao.getByUser(userId);
    } catch (const std::exception& e) {
        std::cerr << "Error in getNotificationSettings: " << e.what() << std::endl;
        return {};
    }
}

bool NotificationService::updateNotificationSetting(unsigned int userId, unsigned int categoryId, bool enabled) {
    try {
        if (enabled) {
            auto settings = settingDao.getByUser(userId);
            for (const auto& setting : settings) {
                if (setting->categoryId == categoryId) {
                    return true;
                }
            }
            
            auto newSetting = std::make_shared<NotificationSetting>(userId, categoryId);
            return settingDao.add(*newSetting);
        } else {
            return settingDao.remove(userId, categoryId);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in updateNotificationSetting: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> NotificationService::getKeywords(unsigned int userId) {
    try {
        return keywordDao.getByUser(userId);
    } catch (const std::exception& e) {
        std::cerr << "Error in getKeywords: " << e.what() << std::endl;
        return {};
    }
}

bool NotificationService::addKeyword(unsigned int userId, const std::string& keyword) {
    try {
        return keywordDao.add(userId, keyword);
    } catch (const std::exception& e) {
        std::cerr << "Error in addKeyword: " << e.what() << std::endl;
        return false;
    }
}

bool NotificationService::removeKeyword(unsigned int userId, const std::string& keyword) {
    try {
        return keywordDao.remove(userId, keyword);
    } catch (const std::exception& e) {
        std::cerr << "Error in removeKeyword: " << e.what() << std::endl;
        return false;
    }
}

bool NotificationService::createNotification(unsigned int userId, unsigned int articleId) {
    try {
        auto notifications = notificationDao.getNotificationsByUser(userId);
        for (const auto& notification : notifications) {
            if (notification->articleId == articleId) {
                return true;
            }
        }
        
        auto notification = std::make_shared<Notification>(userId, articleId, "unseen");
        bool success = notificationDao.add(*notification);
        
        if (success) {
            auto user = userDao.findById(userId);
            
            if (user && !user->emailId.empty()) {
                auto article = articleDao.findById(articleId);
                
                if (article) {
                    std::vector<std::string> notificationMessages;
                    notificationMessages.push_back("New article: " + article->title);
                    
                    return sendEmailNotification(userId, {notification});
                }
            }
        }
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Error in createNotification: " << e.what() << std::endl;
        return false;
    }
}

void NotificationService::processArticleForNotifications(unsigned int articleId) {
    try {
        auto article = articleDao.findById(articleId);
        
        if (!article) {
            std::cerr << "Article not found: " << articleId << std::endl;
            return;
        }
        
        auto categorySettings = settingDao.getByCategoryId(article->categoryId);
        
        for (const auto& setting : categorySettings) {
            createNotification(setting->userId, articleId);
        }
        
        auto allKeywords = keywordDao.getAll();
        std::string articleTitleLower = toLowerCase(article->title);
        std::string articleDescriptionLower = toLowerCase(article->description);
        
        for (const auto& entry : allKeywords) {
            unsigned int userId = entry.first;
            std::vector<std::string> userKeywords = entry.second;
            
            bool keywordMatch = false;
            for (const std::string& keyword : userKeywords) {
                std::string keywordLower = toLowerCase(keyword);
                
                if (articleTitleLower.find(keywordLower) != std::string::npos ||
                    articleDescriptionLower.find(keywordLower) != std::string::npos) {
                    keywordMatch = true;
                    break;
                }
            }
            
            if (keywordMatch) {
                createNotification(userId, articleId);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in processArticleForNotifications: " << e.what() << std::endl;
    }
}

bool NotificationService::sendEmailNotification(unsigned int userId, const std::vector<std::shared_ptr<Notification>>& notifications) {
    try {
        auto user = userDao.findById(userId);
        
        if (!user || user->emailId.empty()) {
            std::cerr << "User not found or email is empty for user: " << userId << std::endl;
            return false;
        }
        
        std::vector<std::string> notificationMessages;
        
        for (const auto& notification : notifications) {
            auto article = articleDao.findById(notification->articleId);
            if (article) {
                std::string msg = "New article: " + article->title + "\nURL: " + article->url;
                notificationMessages.push_back(msg);
            }
        }
        
        if (notificationMessages.empty()) {
            std::cerr << "No valid notifications to send for user: " << userId << std::endl;
            return false;
        }
        
        return EmailService::sendNotificationEmail(
            user->emailId,
            "News Aggregator - You have " + std::to_string(notificationMessages.size()) + " new notifications",
            notificationMessages
        );
    } catch (const std::exception& e) {
        std::cerr << "Error in sendEmailNotification: " << e.what() << std::endl;
        return false;
    }
}

std::vector<unsigned int> NotificationService::getUsersInterestedInArticle(unsigned int articleId) {
    std::vector<unsigned int> interestedUsers;
    std::set<unsigned int> uniqueUsers; // To avoid duplicate users
    
    try {
        auto article = articleDao.findById(articleId);
        
        if (!article) {
            std::cerr << "Article not found: " << articleId << std::endl;
            return interestedUsers;
        }
        
        // Get users interested in this category
        auto categorySettings = settingDao.getByCategoryId(article->categoryId);
        for (const auto& setting : categorySettings) {
            uniqueUsers.insert(setting->userId);
        }
        
        // Get users interested in keywords from this article
        auto allKeywords = keywordDao.getAll();
        std::string articleTitleLower = toLowerCase(article->title);
        std::string articleDescriptionLower = toLowerCase(article->description);
        
        for (const auto& entry : allKeywords) {
            unsigned int userId = entry.first;
            std::vector<std::string> userKeywords = entry.second;
            
            for (const std::string& keyword : userKeywords) {
                std::string keywordLower = toLowerCase(keyword);
                
                if (articleTitleLower.find(keywordLower) != std::string::npos ||
                    articleDescriptionLower.find(keywordLower) != std::string::npos) {
                    uniqueUsers.insert(userId);
                    break;
                }
            }
        }
        
        // Convert set to vector
        interestedUsers.assign(uniqueUsers.begin(), uniqueUsers.end());
        
    } catch (const std::exception& e) {
        std::cerr << "Error in getUsersInterestedInArticle: " << e.what() << std::endl;
    }
    
    return interestedUsers;
}
