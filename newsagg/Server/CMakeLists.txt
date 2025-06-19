cmake_minimum_required(VERSION 3.10)
project(NewsAggServer VERSION 1.0)

# Find required packages
find_package(Threads REQUIRED)

# Server library
add_library(httpserver
    Http/Src/HttpServer.cpp
)

# Include directories - directly reference Common from parent directory
target_include_directories(httpserver PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Http/Inc
)

# Create server executable
add_executable(newsagg_server server_main.cpp)

# Link libraries
target_link_libraries(newsagg_server 
    PRIVATE 
    httpserver
    Threads::Threads
)

# Install rules
install(TARGETS newsagg_server RUNTIME DESTINATION bin)

# Build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
