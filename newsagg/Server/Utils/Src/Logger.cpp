#include "../Inc/Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Logger::info(const std::string& message) {
    std::cout << "[INFO] [" << getCurrentTimestamp() << "] " << message << std::endl;
}

void Logger::error(const std::string& message) {
    std::cerr << "[ERROR] [" << getCurrentTimestamp() << "] " << message << std::endl;
}

void Logger::debug(const std::string& message) {
    std::cout << "[DEBUG] [" << getCurrentTimestamp() << "] " << message << std::endl;
}

void Logger::warning(const std::string& message) {
    std::cout << "[WARNING] [" << getCurrentTimestamp() << "] " << message << std::endl;
}
