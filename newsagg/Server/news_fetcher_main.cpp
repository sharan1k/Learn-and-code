#include "NewsSources/Inc/NewsSourceManager.h"
#include "Dao/Inc/DbConnection.h"
#include "Config.h"
#include <iostream>
#include <string>
#include <csignal>
#include <thread>
#include <chrono>
#include <atomic>

std::atomic<bool> running = true;

void signalHandler(int signum) {
    std::cout << "Signal received (" << signum << "). Shutting down news fetcher..." << std::endl;
    NewsSourceManager::getInstance().stopFetchingNews();
    running = false;
}

void initDbConnection() {
    const std::string& dbHost = Config::DATABASE_HOST;
    const std::string& dbUser = Config::DATABASE_USER;
    const std::string& dbPassword = Config::DATABASE_PASSWORD;
    const std::string& dbSchema = Config::DATABASE_SCHEMA;
 
    std::cout << "Initializing database connection to " << dbHost << "..." << std::endl;
    try {
        DbConnection::initDbConnection(dbHost, dbUser, dbPassword, dbSchema);
        std::cout << "Database connection initialized successfully." << std::endl;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        std::cerr << "ErrorCode: " << e.getErrorCode() << std::endl;
        std::cerr << "News fetcher will exit due to database connectivity failure." << std::endl;
        exit(1);
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "News fetcher will exit due to database connectivity failure." << std::endl;
        exit(1);
    }
}

int main(int argc, char** argv) {
    std::cout << "NewsAgg News Fetcher Application" << std::endl;
    std::cout << "=================================" << std::endl;
    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    initDbConnection();
    
    int fetchIntervalMinutes = Config::NEWS_FETCH_INTERVAL_MINUTES;
    
    if (argc >= 2) {
        try {
            fetchIntervalMinutes = std::stoi(argv[1]);
            std::cout << "Using custom fetch interval: " << fetchIntervalMinutes << " minutes" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Invalid interval: " << argv[1] << ". Using default: " 
                      << fetchIntervalMinutes << " minutes" << std::endl;
        }
    }
    
    auto& newsManager = NewsSourceManager::getInstance();
    newsManager.loadNewsSourcesFromDatabase();
    
    std::string timeMessage;
    if (fetchIntervalMinutes >= 60 && fetchIntervalMinutes % 60 == 0) {
        int hours = fetchIntervalMinutes / 60;
        timeMessage = std::to_string(hours) + "-hour" + (hours > 1 ? "s" : "");
    } else {
        timeMessage = std::to_string(fetchIntervalMinutes) + "-minute" + (fetchIntervalMinutes > 1 ? "s" : "");
    }
    
    std::cout << "News Fetcher starting with " << timeMessage << " interval" << std::endl;
    
    std::cout << "Performing initial news fetch..." << std::endl;
    bool success = newsManager.fetchNewsNow();
    std::cout << "Initial news fetch " << (success ? "completed successfully" : "had some failures") << std::endl;
    
    while (running) {
        std::cout << "Next news fetch scheduled in " << fetchIntervalMinutes << " minutes" << std::endl;
        
        for (int i = 0; i < fetchIntervalMinutes && running; i++) {
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
        
        if (running) {
            std::cout << "Performing scheduled news fetch..." << std::endl;
            success = newsManager.fetchNewsNow();
            std::cout << "Scheduled news fetch " << (success ? "completed successfully" : "had some failures") << std::endl;
        }
    }
    
    std::cout << "News Fetcher shutting down" << std::endl;
    return 0;
}
