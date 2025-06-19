cmake_minimum_required(VERSION 3.10)
project(NewsAggClient VERSION 1.0)

# Find required packages
find_package(Threads REQUIRED)

# Client library
add_library(httpclient
    Http/Src/HttpClient.cpp
)

# Include directories - directly reference Common from parent directory
target_include_directories(httpclient PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Http/Inc
)

# Create client executable
add_executable(newsagg_client client_main.cpp)

# Link libraries
target_link_libraries(newsagg_client 
    PRIVATE 
    httpclient
    Threads::Threads
)

# Install rules
install(TARGETS newsagg_client RUNTIME DESTINATION bin)

# Build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
