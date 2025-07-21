#ifndef CLIENT_APPLICATION_H
#define CLIENT_APPLICATION_H

#include "../../Handlers/Inc/AuthenticationHandler.h"
#include "../../Handlers/Inc/AdminHandler.h"
#include "../../Handlers/Inc/ArticleHandler.h"
#include "../../Handlers/Inc/NotificationHandler.h"
#include "../../Http/Inc/HttpClient.h"
#include <memory>
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <ctime>
#include <iomanip>
#include <sstream>

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
    
    void showHeadlinesMenu();
    void showHeadlinesByDateMenu();
    void showCategoriesMenu(const std::string& startDate = "", const std::string& endDate = "");
    void displayHeadlines(const std::vector<Article*>& articles);
    void displaySavedArticles(const std::vector<Article*>& articles);
    void handleArticleDetails(unsigned int articleId);
    void saveArticle(unsigned int articleId);
    void likeArticle(unsigned int articleId);
    void dislikeArticle(unsigned int articleId);
    void reportArticle(unsigned int articleId);
    void removeSavedArticle(unsigned int articleId);
    void showSavedArticlesMenu();
    void handleSearchArticles();
    void showNotificationsMenu();
    void viewNotifications();
    void configureNotifications();
    void showNotificationCategoryMenu();
    void configureKeywords();
    
    void showContentModerationMenu();
    void viewReportedArticles();
    void viewHiddenArticles();
    void viewHiddenCategories();
    void hideArticle(unsigned int articleId);
    void unhideArticle(unsigned int articleId);
    void hideCategory(unsigned int categoryId);
    void unhideCategory(unsigned int categoryId);
    
    bool waitForRequestCompletion(bool& requestComplete);
    void displayMenuHeader(const std::string& title);
    std::string getCurrentDateString();
    std::string getCurrentTimeString();
    bool validateDateFormat(const std::string& date);
    
    std::string getCategoryNameById(unsigned int categoryId);
    void loadCategories();
    void displayUnseenNotifications(const std::vector<Notification*>& notifications);
    
    std::shared_ptr<HttpClient> httpClient;
    std::unique_ptr<AuthenticationHandler> authenticationHandler;
    std::unique_ptr<AdminHandler> adminHandler;
    std::unique_ptr<ArticleHandler> articleHandler;
    std::unique_ptr<NotificationHandler> notificationHandler;
    User* currentUser;
    bool isLoggedIn;
    bool isRunning;
    std::map<unsigned int, std::string> categoryMap;
};

#endif // CLIENT_APPLICATION_H
