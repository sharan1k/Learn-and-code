#pragma once

#include "../../../Common/Inc/Dto/Notification.h"
#include "../../../Common/Inc/Dto/NotificationSetting.h"
#include "../../Dao/Inc/NotificationDao.h"
#include "../../Dao/Inc/NotificationSettingDao.h"
#include "../../Dao/Inc/KeywordDao.h"
#include "../../Dao/Inc/ArticleDao.h"
#include "../../Dao/Inc/UserDao.h"
#include <vector>
#include <string>
#include <memory>

class NotificationService {
public:
    NotificationService();
    ~NotificationService();
    std::vector<std::shared_ptr<Notification>> getNotifications(unsigned int userId);
    bool markNotificationsAsSeen(unsigned int userId);    
    std::vector<std::shared_ptr<NotificationSetting>> getNotificationSettings(unsigned int userId);    
    bool updateNotificationSetting(unsigned int userId, unsigned int categoryId, bool enabled);    
    std::vector<std::string> getKeywords(unsigned int userId);    
    bool addKeyword(unsigned int userId, const std::string& keyword);    
    bool removeKeyword(unsigned int userId, const std::string& keyword);    
    bool createNotification(unsigned int userId, unsigned int articleId);    
    void processArticleForNotifications(unsigned int articleId);    
    bool sendEmailNotification(unsigned int userId, const std::vector<std::shared_ptr<Notification>>& notifications);
    std::vector<unsigned int> getUsersInterestedInArticle(unsigned int articleId); // New method

private:
    NotificationDao notificationDao;
    NotificationSettingDao settingDao;
    KeywordDao keywordDao;
    ArticleDao articleDao;
    UserDao userDao;
};
