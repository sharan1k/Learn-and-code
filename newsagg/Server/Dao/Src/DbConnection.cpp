#include "../Inc/DbConnection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>

std::shared_ptr<sql::Connection> DbConnection::connection = nullptr;
std::shared_ptr<DbConnection> DbConnection::instance = nullptr;

DbConnection::DbConnection() {
}

DbConnection::~DbConnection() {
  if (connection) {
    try {
      connection->close();
      std::cout << "Database connection closed." << std::endl;
    } catch (sql::SQLException &exception) {
      std::cerr << "Error closing database connection: " << exception.what() << std::endl;
    }
    connection = nullptr;
  }
}

void DbConnection::initDbConnection(const std::string &hostName,
                                    const std::string &userName,
                                    const std::string &password,
                                    const std::string &schemaName) {
  if (connection) {
    std::cout << "Database connection already initialized." << std::endl;
    return;
  }

  try {
    sql::Driver *driver = get_driver_instance();
    std::cout << "Connecting to MySQL server: " << hostName << "..." << std::endl;
    connection.reset(driver->connect(hostName, userName, password));
    connection->setSchema(schemaName);
    std::cout << "Database connection established successfully." << std::endl;
  } catch (sql::SQLException &exception) {
    std::cerr << "SQLException: " << exception.what() << std::endl;
    std::cerr << "SQLState: " << exception.getSQLState() << std::endl;
    std::cerr << "ErrorCode: " << exception.getErrorCode() << std::endl;
    throw; 
  } catch (std::exception &exception) {
    std::cerr << "Error: " << exception.what() << std::endl;
    throw;
  }
}

std::shared_ptr<DbConnection> DbConnection::getInstance() {
  if (!instance) {
    instance.reset(new DbConnection());
  }
  return instance;
}

std::shared_ptr<sql::Connection> DbConnection::getConnection() {
  if (!connection) {
    throw std::runtime_error("Database connection has not been initialized. Call initDbConnection first.");
  }
  if (!connection->isValid()) {
    throw std::runtime_error("Database connection is invalid or closed.");
  }
  
  return connection;
}
