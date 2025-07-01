#pragma once

#include <string>

namespace Config {
    const std::string THE_NEWS_API_KEY = "k5ssTGZT7yPRwHBUbwDUHyMvb7OMfTZo9QIEHMQD";
    const int SERVER_PORT = 8080;
    const int NEWS_FETCH_INTERVAL_MINUTES = 180;
    const std::string DATABASE_HOST = "tcp://172.24.160.1:3306";
    const std::string DATABASE_USER = "root";
    const std::string DATABASE_PASSWORD = "your_password";
    const std::string DATABASE_SCHEMA = "newsaggregator";
}
