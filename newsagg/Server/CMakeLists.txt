cmake_minimum_required(VERSION 3.10)
project(NewsAggServer VERSION 1.0)

# Find required packages
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(CURL REQUIRED)

# Find MySQL Connector/C++
find_path(MYSQLCPPCONN_INCLUDE_DIR NAMES cppconn/driver.h
    PATHS
    /usr/include
    /usr/include/mysql
    /usr/local/include
    /usr/local/include/mysql
    /usr/include/mysql-cppconn-8
    /usr/local/include/mysql-cppconn-8
)

# Find nlohmann/json.hpp
find_path(NLOHMANN_JSON_INCLUDE_DIR NAMES nlohmann/json.hpp
    PATHS
    /usr/include
    /usr/local/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
)

find_library(MYSQLCPPCONN_LIBRARY NAMES mysqlcppconn
    PATHS
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
)

if(MYSQLCPPCONN_INCLUDE_DIR AND MYSQLCPPCONN_LIBRARY)
    message(STATUS "MySQL Connector/C++ found:")
    message(STATUS "  Include: ${MYSQLCPPCONN_INCLUDE_DIR}")
    message(STATUS "  Library: ${MYSQLCPPCONN_LIBRARY}")
else()
    message(FATAL_ERROR "MySQL Connector/C++ not found. Please install the MySQL Connector/C++ development package.")
endif()

if(NLOHMANN_JSON_INCLUDE_DIR)
    message(STATUS "nlohmann/json found: ${NLOHMANN_JSON_INCLUDE_DIR}")
else()
    message(STATUS "nlohmann/json not found. Downloading...")
    include(FetchContent)
    FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(json)
    set(NLOHMANN_JSON_INCLUDE_DIR ${json_SOURCE_DIR}/single_include)
    message(STATUS "Downloaded nlohmann/json: ${NLOHMANN_JSON_INCLUDE_DIR}")
endif()

# Server library
add_library(httpserver
    Http/Src/HttpServer.cpp
)

# Service library
add_library(service
    Service/Src/UserService.cpp
    Service/Src/ExternalServerService.cpp
    Service/Src/CategoryService.cpp
    Service/Src/ArticleService.cpp
    Service/Src/NotificationService.cpp
    Service/Src/AdminService.cpp
)

# Controller library
add_library(controller
    Controller/Src/UserController.cpp
    Controller/Src/AdminController.cpp
    Controller/Src/ArticleController.cpp
    Controller/Src/NotificationController.cpp
    Controller/Src/AdminControllerHiddenContent.cpp
    Controller/Src/ArticleControllerReport.cpp
)

# Include directories - directly reference Common from parent directory
target_include_directories(httpserver PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Http/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Dao/Inc
    ${MYSQLCPPCONN_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

# Include directories for service
target_include_directories(service PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Http/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Dao/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Controller/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Service/Inc
    ${MYSQLCPPCONN_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

# Include directories for controller
target_include_directories(controller PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Http/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Dao/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Controller/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Service/Inc
    ${MYSQLCPPCONN_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

# Link controller with service
target_link_libraries(controller
    service
)

# Link service with dbconnection
target_link_libraries(service
    dbconnection
)

# News sources library
add_library(newssources
    NewsSources/Src/TheNewsApi.cpp
    NewsSources/Src/NewsApi.cpp
    NewsSources/Src/NewsSourceManager.cpp
)

# Database connection library
add_library(dbconnection
    Dao/Src/DbConnection.cpp
    Dao/Src/UserDao.cpp
    Dao/Src/ArticleDao.cpp
    Dao/Src/CategoryDao.cpp
    Dao/Src/ExternalServerDao.cpp
    Dao/Src/SavedArticleDao.cpp
    Dao/Src/LikesDao.cpp
    Dao/Src/NotificationDao.cpp
    Dao/Src/NotificationSettingDao.cpp
    Dao/Src/KeywordDao.cpp
    Dao/Src/ReportedArticleDao.cpp
    Dao/Src/HiddenArticleDao.cpp
    Dao/Src/HiddenCategoryDao.cpp
)

# Utils library
add_library(utils
    Utils/Src/EmailService.cpp
    Utils/Src/Logger.cpp
)

target_include_directories(utils PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CURL_INCLUDE_DIRS}
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

target_link_libraries(utils PUBLIC 
    ${CURL_LIBRARIES}
    -lcurl
)

target_include_directories(dbconnection PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Dao/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${MYSQLCPPCONN_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

target_link_libraries(dbconnection
    ${MYSQLCPPCONN_LIBRARY}
)

# Create server executable
add_executable(newsagg_server serverMain.cpp)

# Create news fetcher executable
add_executable(newsagg_fetcher newsFetcherMain.cpp)

# Link libraries
target_include_directories(newssources PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/NewsSources/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Dao/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${MYSQLCPPCONN_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

target_link_libraries(newssources
    dbconnection
    utils
    ${CURL_LIBRARIES}
)

# Update target include directories for newsagg_server to include Config
target_include_directories(newsagg_server PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Config/Inc
)

# Link libraries for newsagg_server
target_link_libraries(newsagg_server 
    PRIVATE 
    controller  # controller depends on service and dbconnection
    httpserver
    service
    newssources
    dbconnection
    utils
    Threads::Threads
    ${MYSQLCPPCONN_LIBRARY}
    OpenSSL::SSL
    OpenSSL::Crypto
    ${CURL_LIBRARIES}
)

# Link libraries for newsagg_fetcher
target_link_libraries(newsagg_fetcher
    PRIVATE
    newssources
    dbconnection
    service
    utils
    Threads::Threads
    ${MYSQLCPPCONN_LIBRARY}
    OpenSSL::SSL
    OpenSSL::Crypto
    ${CURL_LIBRARIES}
    -lcurl
)

# Add include directories for newsagg_fetcher
target_include_directories(newsagg_fetcher PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Config/Inc
    ${CURL_INCLUDE_DIRS}
)

# Add include directories for the newsagg_fetcher specifically to find the CURL headers
target_include_directories(newsagg_fetcher PUBLIC
    /usr/include
    /usr/local/include
)

# Make sure CURL headers are included for newssources which may use notification services
target_include_directories(newssources PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/NewsSources/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Dao/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Service/Inc
    ${MYSQLCPPCONN_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
)

# Install rules
install(TARGETS newsagg_server newsagg_fetcher RUNTIME DESTINATION bin)

# Build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler definitions for OpenSSL support
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)
