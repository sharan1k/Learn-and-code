#include "NewsSources/Inc/NewsSourceManager.h"
#include "Dao/Inc/DbConnection.h"
#include "Utils/Inc/Logger.h"
#include "Config.h"
#include <string>
#include <csignal>
#include <thread>
#include <chrono>
#include <atomic>

std::atomic<bool> running = true;
std::string formatTimeMessage(int minutes);

void signalHandler(int signum) {
    Logger::info("Signal received (" + std::to_string(signum) + "). Shutting down news fetcher...");
    NewsSourceManager::getInstance().stopFetchingNews();
    running = false;
}

void initDbConnection() {
    const std::string& dbHost = Config::DATABASE_HOST;
    const std::string& dbUser = Config::DATABASE_USER;
    const std::string& dbSchema = Config::DATABASE_SCHEMA;
 
    Logger::info("Initializing database connection to " + dbHost + "...");
    try {
        DbConnection::initDbConnection(dbHost, dbUser, Config::DATABASE_PASSWORD, dbSchema);
        Logger::info("Database connection initialized successfully");
    } catch (sql::SQLException &e) {
        Logger::error("SQLException: " + std::string(e.what()));
        Logger::error("SQLState: " + std::string(e.getSQLState()));
        Logger::error("ErrorCode: " + std::to_string(e.getErrorCode()));
        Logger::error("News fetcher will exit due to database connectivity failure");
        exit(1);
    } catch (std::exception &e) {
        Logger::error("Error: " + std::string(e.what()));
        Logger::error("News fetcher will exit due to database connectivity failure");
        exit(1);
    }
}

int main(int argc, char** argv) {
    Logger::info("NewsAgg News Fetcher Application");
    Logger::info("=================================");
    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    initDbConnection();
    
    int fetchIntervalMinutes = Config::NEWS_FETCH_INTERVAL_MINUTES;
    
    if (argc >= 2) {
        try {
            fetchIntervalMinutes = std::stoi(argv[1]);
            Logger::info("Using custom fetch interval: " + std::to_string(fetchIntervalMinutes) + " minutes");
        } catch (const std::exception& e) {
            Logger::warning("Invalid interval: " + std::string(argv[1]) + 
                          ". Using default: " + std::to_string(fetchIntervalMinutes) + " minutes");
        }
    }
    
    auto& newsManager = NewsSourceManager::getInstance();
    newsManager.loadNewsSourcesFromDatabase();
    
    std::string timeMessage = formatTimeMessage(fetchIntervalMinutes);
    Logger::info("News Fetcher starting with " + timeMessage + " interval");
    
    Logger::info("Performing initial news fetch...");
    bool success = newsManager.fetchNewsNow();
    Logger::info("Initial news fetch " + std::string(success ? "completed successfully" : "had some failures"));
    
    while (running) {
        Logger::info("Next news fetch scheduled in " + std::to_string(fetchIntervalMinutes) + " minutes");
        
        for (int i = 0; i < fetchIntervalMinutes && running; i++) {
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
        
        if (running) {
            Logger::info("Performing scheduled news fetch...");
            success = newsManager.fetchNewsNow();
            Logger::info("Scheduled news fetch " + std::string(success ? "completed successfully" : "had some failures"));
        }
    }
    
    Logger::info("News Fetcher shutting down");
    return 0;
}

std::string formatTimeMessage(int minutes) {
    if (minutes >= 60 && minutes % 60 == 0) {
        int hours = minutes / 60;
        return std::to_string(hours) + "-hour" + (hours > 1 ? "s" : "");
    } else {
        return std::to_string(minutes) + "-minute" + (minutes > 1 ? "s" : "");
    }
}
