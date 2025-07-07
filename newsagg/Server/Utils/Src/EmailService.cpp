#include "../Inc/EmailService.h"
#include "../Inc/Logger.h"
#include <curl/curl.h>
#include <iostream>
#include <cstring>
#include <sstream>

bool EmailService::initializeEmailService() {
    curl_global_init(CURL_GLOBAL_ALL);
    return true;
}

bool EmailService::sendEmail(
    const std::string& to_email,
    const std::string& subject,
    const std::string& body
) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "CURL initialization failed!" << std::endl;
        return false;
    }

    std::string payload_text = prepare_email_payload(Config::EMAIL_FROM, to_email, subject, body);

    bool result = setup_smtp_connection(curl, Config::SMTP_SERVER, Config::EMAIL_FROM, to_email, payload_text);
    curl_easy_cleanup(curl);
    return result;
}

bool EmailService::sendNotificationEmail(
    const std::string& to_email,
    const std::string& subject,
    const std::vector<std::string>& notifications
) {
    std::stringstream bodyStream;
    bodyStream << "Hello,\r\n\r\nYou have " << notifications.size() << " new notifications:\r\n\r\n";
    
    for (size_t i = 0; i < notifications.size(); ++i) {
        std::string::size_type pos = notifications[i].find(",");
        if (pos != std::string::npos) {
            std::string title = notifications[i].substr(0, pos);
            std::string url = notifications[i].substr(pos + 1);
            bodyStream << (i + 1) << ". " << title << " (" << url << ")\r\n";
        } else {
            bodyStream << (i + 1) << ". " << notifications[i] << "\r\n";
        }
    }
    
    bodyStream << "\r\n\r\nRegards,\r\nNews Aggregator Team";
    
    return sendEmail(to_email, subject, bodyStream.str());
}

std::string EmailService::prepare_email_payload(
    const std::string& from_email, 
    const std::string& to_email, 
    const std::string& subject, 
    const std::string& body
) {
    return "To: " + to_email + "\r\n"
           "From: " + from_email + "\r\n"
           "Subject: " + subject + "\r\n"
           "\r\n" + body + "\r\n";
}

size_t payload_source(char* ptr, size_t size, size_t nmemb, void* userp) {
    const char** payload = reinterpret_cast<const char**>(userp);
    if (*payload) {
        size_t len = strlen(*payload);
        memcpy(ptr, *payload, len);
        *payload = nullptr;
        return len;
    }
    return 0;
}

bool EmailService::setup_smtp_connection(
    void* curlPtr, 
    const std::string& smtp_server, 
    const std::string& from_email, 
    const std::string& to_email, 
    const std::string& payload
) {
    CURL* curl = static_cast<CURL*>(curlPtr);
    struct curl_slist* recipients = nullptr;
    const char* payload_cstr = payload.c_str();

    curl_easy_setopt(curl, CURLOPT_USERNAME, Config::EMAIL_FROM.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, Config::EMAIL_APP_PASSWORD.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, smtp_server.c_str());
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + from_email + ">").c_str());
    recipients = curl_slist_append(recipients, ("<" + to_email + ">").c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &payload_cstr);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(recipients);

    if (res != CURLE_OK) {
        std::cerr << "Email send failed: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    std::cout << "Email sent successfully to " << to_email << std::endl;
    return true;
}
