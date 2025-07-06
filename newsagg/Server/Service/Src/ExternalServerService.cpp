#include "../Inc/ExternalServerService.h"

ExternalServerService::ExternalServerService() {}

ExternalServerService::~ExternalServerService() {}

std::vector<std::shared_ptr<ExternalServer>> ExternalServerService::getAllExternalServers() {
    return serverDao.getAllExternalServers();
}

std::shared_ptr<ExternalServer> ExternalServerService::getExternalServerById(unsigned int serverId) {
    return serverDao.findById(serverId);
}

bool ExternalServerService::updateExternalServer(const ExternalServer& server) {
    return serverDao.updateExternalServer(server);
}
