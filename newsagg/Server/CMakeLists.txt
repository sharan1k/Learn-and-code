cmake_minimum_required(VERSION 3.10)
project(NewsAggServer VERSION 1.0)

# Find required packages
find_package(Threads REQUIRED)

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
)

# Controller library
add_library(controller
    Controller/Src/UserController.cpp
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

# Database connection library
add_library(dbconnection
    Dao/Src/DbConnection.cpp
    Dao/Src/UserDao.cpp
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
add_executable(newsagg_server server_main.cpp)

# Link libraries
target_link_libraries(newsagg_server 
    PRIVATE 
    controller  # controller depends on service and dbconnection
    httpserver
    service
    dbconnection
    Threads::Threads
    ${MYSQLCPPCONN_LIBRARY}
)

# Install rules
install(TARGETS newsagg_server RUNTIME DESTINATION bin)

# Build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
