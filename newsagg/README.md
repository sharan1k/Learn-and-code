# NewsAgg - News Aggregator

A news aggregator system with completely separate client and server applications.

## Building with CMake

This project is structured as two independent applications that share common code.

### Prerequisites

- CMake (minimum version 3.10)
- C++ compiler with C++17 support
- pthread library

### Build Instructions (Using CMake directly)

#### Building the Server

```bash
# Navigate to the Server directory
cd Server

# Create a build directory
mkdir -p build
cd build

# Configure the project
cmake ..

# Build the project
# For single-configuration generators (Make, Ninja)
cmake --build .

# For multi-configuration generators (Visual Studio)
cmake --build . --config Release
```

#### Building the Client

```bash
# Navigate to the Client directory
cd Client

# Create a build directory
mkdir -p build
cd build

# Configure the project
cmake ..

# Build the project
# For single-configuration generators (Make, Ninja)
cmake --build .

# For multi-configuration generators (Visual Studio)
cmake --build . --config Release
```

### Running the Applications

1. **Run the server:**

```bash
# From the Server build directory
./newsagg_server [port]
```

2. **Run the client:**

```bash
# From the Client build directory
./newsagg_client [host] [port]
```

By default, the server runs on port 8080 and the client connects to localhost:8080.

## Testing the HTTP Module

The current implementation includes basic HTTP client and server functionality:

- The server exposes test endpoints at `/api/test` for both GET and POST methods
- The client can make GET and POST requests to these endpoints

When you run both applications, the client will automatically test the communication with the server.

## Project Structure

```
NewsAgg/
├── Client/
│   ├── ClientApplication/
│   ├── Handlers/
│   ├── Http/
│   │   ├── Inc/
│   │   │   └── HttpClient.h
│   │   └── Src/
│   │       └── HttpClient.cpp
│   └── client_main.cpp
├── Common/
│   ├── Inc/
│   │   └── httplib.h
│   └── Src/
└── Server/
    ├── Controller/
    ├── Dao/
    ├── Http/
    │   ├── Inc/
    │   │   └── HttpServer.h
    │   └── Src/
    │       └── HttpServer.cpp
    ├── Service/
    └── server_main.cpp
```
