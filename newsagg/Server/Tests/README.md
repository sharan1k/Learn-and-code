# News Aggregator Service Unit Tests

This directory contains unit tests for the News Aggregator service layer.

## Overview

The tests use Google Test (gtest) and Google Mock (gmock) frameworks to create unit tests for:

- UserService
- ExternalServerService
- CategoryService

## How to Run

1. Navigate to the `Tests` directory
2. Create a build directory: `mkdir build && cd build`
3. Run cmake: `cmake ..`
4. Build the tests: `cmake --build .`
5. Run the tests: `ctest` or directly run `./service_tests`

## Test Structure

- `UserServiceTest.cpp` - Tests for user registration, authentication, and profile management
- `ExternalServerServiceTest.cpp` - Tests for external server management
- `CategoryServiceTest.cpp` - Tests for category management

Each test file uses a corresponding mock DAO class to isolate the service layer for testing.
