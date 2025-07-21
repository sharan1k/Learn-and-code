#include <iostream>
#include <chrono>
#include "ClientApplication.h"

std::string ClientApplication::getCurrentDateString() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
    
    #ifdef _WIN32
    localtime_s(&tm_now, &now_time);
    #else
    localtime_r(&now_time, &tm_now);
    #endif
    
    char buffer[12];
    std::strftime(buffer, sizeof(buffer), "%d-%b-%Y", &tm_now);
    return std::string(buffer);
}

std::string ClientApplication::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
    
    #ifdef _WIN32
    localtime_s(&tm_now, &now_time);
    #else
    localtime_r(&now_time, &tm_now);
    #endif
    
    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%I:%M%p", &tm_now);
    return std::string(buffer);
}

bool ClientApplication::validateDateFormat(const std::string& date) {
    std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
    return std::regex_match(date, dateRegex);
}
