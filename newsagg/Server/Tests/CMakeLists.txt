cmake_minimum_required(VERSION 3.10)
project(newsagg_tests)

# Google Test requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/../Dao/Inc
    ${CMAKE_SOURCE_DIR}/../Service/Inc
    ${CMAKE_SOURCE_DIR}/../../Common/Inc
)

# Add test executable
add_executable(service_tests
    UserServiceTest.cpp
    ExternalServerServiceTest.cpp
    CategoryServiceTest.cpp
    Mocks/MockUserDao.h
    Mocks/MockExternalServerDao.h
    Mocks/MockCategoryDao.h
)

# Link against the needed libraries
target_link_libraries(service_tests
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(service_tests)
