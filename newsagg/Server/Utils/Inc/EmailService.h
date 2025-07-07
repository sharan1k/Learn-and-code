#pragma once

#include <string>
#include <vector>
#include "../../Config.h"

class EmailService {
public:
    static bool initializeEmailService();
    
    static bool sendEmail(
        const std::string& to_email,
        const std::string& subject,
        const std::string& body
    );
    
    static bool sendNotificationEmail(
        const std::string& to_email,
        const std::string& subject,
        const std::vector<std::string>& notifications
    );
    
private:
    
    static std::string prepare_email_payload(
        const std::string& from_email, 
        const std::string& to_email, 
        const std::string& subject, 
        const std::string& body
    );
    
    static bool setup_smtp_connection(
        void* curl, 
        const std::string& smtp_server, 
        const std::string& from_email, 
        const std::string& to_email, 
        const std::string& payload
    );
};
