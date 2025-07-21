#pragma once

#include "../../../Common/Inc/Dto/NotificationSetting.h"
#include <vector>
#include <memory>

class NotificationSettingDao {
public:
    std::vector<std::shared_ptr<NotificationSetting>> getByUser(unsigned int userId);
    std::vector<std::shared_ptr<NotificationSetting>> getByCategoryId(unsigned int categoryId);
    bool add(const NotificationSetting& setting);
    bool remove(unsigned int userId, unsigned int categoryId);
};
