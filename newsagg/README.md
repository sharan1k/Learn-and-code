# NewsAgg - News Aggregator

A news aggregator system with completely separate client and server applications.

## Building with CMake

This project is structured as independent applications that share common code.

### Prerequisites

- CMake (minimum version 3.10)
- C++ compiler with C++17 support
- pthread library
- MySQL Connector/C++
- OpenSSL for secure connections

### Build Instructions (Using CMake directly)

#### Building the Server Components

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

## Running the Application

The build process generates three executables:

1. `newsagg_client` - Client application that connects to the server
2. `newsagg_server` - Server application that handles API requests from clients
3. `newsagg_fetcher` - News fetching application that periodically retrieves news from external sources

### Server

```bash
cd Server/build
./newsagg_server [port]
```

Where `[port]` is an optional parameter to specify the server port (default: 8080).

### News Fetcher

```bash
cd Server/build
./newsagg_fetcher [interval]
```

Where `[interval]` is an optional parameter to specify the fetch interval in minutes (default: from Config::NEWS_FETCH_INTERVAL_MINUTES).

### Client

```bash
cd Client/build
./newsagg_client [server_address] [port]
```

Where `[server_address]` and `[port]` are optional parameters to specify the server address and port (default: localhost:8080).

## Architecture

The project is structured in three main components:

1. **Client Application**: Handles user interactions and displays data
2. **Server Application**: Processes API requests from clients and serves data from the database
3. **News Fetcher Application**: Periodically fetches news from external APIs and stores them in the database

This separation ensures:

- The server remains highly responsive to client requests
- News fetching operations don't affect client-server communication
- Each component can be scaled and maintained independently

The server and news fetcher share the same database, allowing them to work together without direct communication.

## Configuration

Configuration parameters are defined in `Server/Config.h` and include:

- Database connection settings
- API keys for news sources
- Server settings (port, thread pool size, timeouts)
- News fetching intervals

