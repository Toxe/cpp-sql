#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include "date.h"
#include "tz.h"
#include "performance.h"

using namespace std::chrono_literals;

void warmup(const std::string& filename)
{
    std::ifstream in{filename};
    std::string line;

    std::cout << "* Warmup (" << filename << ")...\n";

    while (std::getline(in, line))
        ;
}

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

void import_data_combined_inserts(sqlpp::mysql::connection& db, const std::string& filename)
{
    std::cout << "* Import data...\n";

    std::ifstream in{filename};
    int count = 0;
    auto time = date::sys_seconds{};
    auto t0 = std::chrono::high_resolution_clock::now();

    Performance::Performance performance{};

    while (in) {
        std::string s1;  if (!std::getline(in, s1, '\t')) break;
        std::string s2;  if (!std::getline(in, s2, '\t')) break;
        std::string s3;  if (!std::getline(in, s3, '\t')) break;
        std::string s4;  if (!std::getline(in, s4, '\t')) break;
        std::string s5;  if (!std::getline(in, s5, '\t')) break;
        std::string s6;  if (!std::getline(in, s6, '\t')) break;
        std::string s7;  if (!std::getline(in, s7))       break;

        std::istringstream{s1} >> date::parse("%F %T", time);

        db(sqlpp::insert_into(performance).set(
            performance.time = time,
            performance.request = std::stoi(s2),
            performance.duration = std::stoi(s3),
            performance.user = std::stoi(s4),
            performance.ipAddress = s5,
            performance.action = s6,
            performance.userAgent = s7));

        ++count;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << "  --> Imported " << count << " rows in " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";
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

int main(int argc, char* argv[])
{
    if (argc < 2)
        throw std::runtime_error{"missing performance data filename"};

    const std::string performance_data_filename{argv[1]};

    auto config = read_mysql_config("sqlpp11_mysql.json");
    sqlpp::mysql::connection db(config);

    warmup(performance_data_filename);

    drop_table(db, "performance");
    create_table(db, "performance");
    import_data_combined_inserts(db, performance_data_filename);
}
