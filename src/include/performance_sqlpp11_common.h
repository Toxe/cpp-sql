#ifndef PERFORMANCE_SQLPP11_COMMON_H_
#define PERFORMANCE_SQLPP11_COMMON_H_

#include <memory>
#include <string>

#include <sqlpp11/mysql/mysql.h>

std::shared_ptr<sqlpp::mysql::connection_config> read_mysql_config(const char* filename);
void warmup(const std::string& filename);
void drop_table(sqlpp::mysql::connection& db, const std::string& table_name);
void create_table(sqlpp::mysql::connection& db, const std::string& table_name);

#endif // PERFORMANCE_SQLPP11_COMMON_H_
