#ifndef PERFORMANCE_MYSQL_CONNECTOR_JDBC_COMMON_H_
#define PERFORMANCE_MYSQL_CONNECTOR_JDBC_COMMON_H_

#include <memory>
#include <string>
#include <mysql/jdbc.h>

sql::ConnectOptionsMap read_mysql_config(const char* filename);
void warmup(const std::string& filename);
void drop_table(sql::Connection* con, const std::string& table_name);
void create_table(sql::Connection* con, const std::string& table_name);
std::unique_ptr<sql::Connection> connect(sql::ConnectOptionsMap& connection_properties);

#endif // PERFORMANCE_MYSQL_CONNECTOR_JDBC_COMMON_H_
