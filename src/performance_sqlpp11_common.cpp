#include "performance_sqlpp11_common.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>

using namespace std::chrono_literals;

// Read JSON config file.
//
// Example "sqlpp11_mysql.json":
//
//   {
//       "user": "username",
//       "password": "password",
//       "database": "ships",
//       "unix_socket": "/tmp/mysql.sock"
//   }
std::shared_ptr<sqlpp::mysql::connection_config> read_mysql_config(const char* filename)
{
    std::ifstream in(filename);
    nlohmann::json data;

    in >> data;
    auto config = std::make_shared<sqlpp::mysql::connection_config>();

    if (!data["host"].empty()) config->host = data["host"].get<std::string>();
    if (!data["user"].empty()) config->user = data["user"].get<std::string>();
    if (!data["password"].empty()) config->password = data["password"].get<std::string>();
    if (!data["database"].empty()) config->database = data["database"].get<std::string>();
    if (!data["unix_socket"].empty()) config->unix_socket = data["unix_socket"].get<std::string>();
    if (!data["charset"].empty()) config->charset = data["charset"].get<std::string>();
    if (!data["port"].empty()) config->port = data["port"].get<unsigned int>();
    if (!data["client_flag"].empty()) config->client_flag = data["client_flag"].get<unsigned long>();
    if (!data["auto_reconnect"].empty()) config->auto_reconnect = data["auto_reconnect"].get<bool>();
    if (!data["debug"].empty()) config->debug = data["debug"].get<bool>();

    return config;
}

void warmup(const std::string& filename)
{
    std::ifstream in{filename};
    std::string line;

    std::cout << "* Warmup (" << filename << ")...\n";

    while (std::getline(in, line))
        ;
}

void drop_table(sqlpp::mysql::connection& db, const std::string& table_name)
{
    std::cout << "DROP TABLE " << table_name << "...\n";

    db.execute("DROP TABLE IF EXISTS " + table_name);
}

void create_table(sqlpp::mysql::connection& db, const std::string& table_name)
{
    std::cout << "CREATE TABLE " << table_name << "...\n";

    db.execute("CREATE TABLE " + table_name + " ("
               "    id           INT NOT NULL AUTO_INCREMENT,"
               "    time         DATETIME NOT NULL,"
               "    request      INT NOT NULL,"
               "    duration     INT NOT NULL,"
               "    user         INT NOT NULL,"
               "    ip_address   VARCHAR(16) NOT NULL,"
               "    action       VARCHAR(64) NOT NULL,"
               "    user_agent   VARCHAR(255) NOT NULL,"
               "    PRIMARY KEY (id)"
               ") CHARSET=utf8 COLLATE=utf8_unicode_ci");

    std::this_thread::sleep_for(500ms);
}
