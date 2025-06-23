#pragma once

#include <cppconn/connection.h>
#include <memory>

class DbConnection {
  static std::shared_ptr<sql::Connection> connection;
  static std::shared_ptr<DbConnection> instance;
  DbConnection();

public:
  DbConnection(const DbConnection &) = delete;
  DbConnection &operator=(const DbConnection &) = delete;
  static void initDbConnection(const std::string &hostName,
                               const std::string &userName,
                               const std::string &password,
                               const std::string &schemaName);
  static std::shared_ptr<DbConnection> getInstance();
  std::shared_ptr<sql::Connection> getConnection();
  ~DbConnection();
};