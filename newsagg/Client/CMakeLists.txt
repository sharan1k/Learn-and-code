cmake_minimum_required(VERSION 3.10)
project(NewsAggClient VERSION 1.0)

# Find required packages
find_package(Threads REQUIRED)

# Check if nlohmann json is available
include(FetchContent)
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

# HTTP Client library
add_library(httpclient
    Http/Src/HttpClient.cpp
)

# User Handler library
add_library(userhandler
    Handlers/Src/UserHandler.cpp
)

# Client Application library
add_library(clientapp
    ClientApplication/Src/ClientApplication.cpp
)

# Include directories
target_include_directories(httpclient PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Http/Inc
)

target_include_directories(userhandler PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Http/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Handlers/Inc
)

target_include_directories(clientapp PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Http/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Handlers/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ClientApplication/Inc
)

# Create client executable
add_executable(newsagg_client client_main.cpp)

# Link libraries
target_link_libraries(userhandler
    PRIVATE
    httpclient
    nlohmann_json::nlohmann_json
)

target_link_libraries(clientapp
    PRIVATE
    userhandler
)

target_link_libraries(newsagg_client 
    PRIVATE 
    httpclient
    userhandler
    clientapp
    Threads::Threads
)

# Install rules
install(TARGETS newsagg_client RUNTIME DESTINATION bin)

# Build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
