#pragma once

#include "../../../Common/Inc/Dto/Notification.h"
#include <vector>
#include <memory>

class NotificationDao {
public:
    std::vector<std::shared_ptr<Notification>> getNotificationsByUser(unsigned int userId);
    bool add(const Notification& notification);
    bool markAllAsSeen(unsigned int userId);
    bool remove(unsigned int userId, unsigned int articleId);
};
