#pragma once

#include <gmock/gmock.h>
#include "../../Dao/Inc/ExternalServerDao.h"

class MockExternalServerDao : public ExternalServerDao {
public:
    MockExternalServerDao() : ExternalServerDao() {}
    ~MockExternalServerDao() override = default;
    
    MOCK_METHOD(bool, createExternalServer, (const ExternalServer&));
    MOCK_METHOD(std::shared_ptr<ExternalServer>, findById, (unsigned int));
    MOCK_METHOD(std::shared_ptr<ExternalServer>, findByName, (const std::string&));
    MOCK_METHOD(bool, updateExternalServer, (const ExternalServer&));
    MOCK_METHOD(bool, deleteExternalServer, (unsigned int));
    MOCK_METHOD(std::vector<std::shared_ptr<ExternalServer>>, getAllExternalServers, ());
    MOCK_METHOD(bool, updateLastAccessed, (unsigned int, const std::string&));
    MOCK_METHOD(bool, setApiStatus, (unsigned int, ApiStatus));
};
