#ifndef USER_HANDLER_H
#define USER_HANDLER_H

#include "../../Http/Inc/HttpClient.h"
#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <functional>

struct User {
    unsigned int userId;
    std::string userName;
    std::string emailId;
    std::string role;
    
    static User fromJson(const nlohmann::json& json);
};

class UserHandler {
public:
    UserHandler(std::shared_ptr<HttpClient> client);
    
    void login(const std::string& userName, 
               const std::string& password,
               std::function<void(bool success, const std::string& message, const User* user)> callback);
               
    void signup(const std::string& userName,
                const std::string& email,
                const std::string& password,
                std::function<void(bool success, const std::string& message, const User* user)> callback);

private:
    std::shared_ptr<HttpClient> client;
    User currentUser;
};

#endif // USER_HANDLER_H
