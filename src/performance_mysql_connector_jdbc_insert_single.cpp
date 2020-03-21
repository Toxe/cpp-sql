#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <mysql/jdbc.h>
#include <nlohmann/json.hpp>

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
// Example "mysql_connector_jdbc.json":
//
//   {
//       "hostName": "localhost",
//       "userName": "username",
//       "password": "password",
//       "schema": "ships",
//       "port": 3306
//   }
sql::ConnectOptionsMap read_mysql_config(const char* filename)
{
    std::ifstream in(filename);
    nlohmann::json data;

    in >> data;
    sql::ConnectOptionsMap connection_properties;

    if (!data["hostName"].empty()) connection_properties["hostName"] = data["hostName"].get<std::string>();
    if (!data["userName"].empty()) connection_properties["userName"] = data["userName"].get<std::string>();
    if (!data["password"].empty()) connection_properties["password"] = data["password"].get<std::string>();
    if (!data["schema"].empty()) connection_properties["schema"] = data["schema"].get<std::string>();
    if (!data["port"].empty()) connection_properties["port"] = data["port"].get<int>();
    if (!data["readDefaultFile"].empty()) connection_properties["readDefaultFile"] = data["readDefaultFile"].get<std::string>();
    if (!data["socket"].empty()) connection_properties["socket"] = data["socket"].get<std::string>();

    return connection_properties;
}

void import(sql::Connection* con, const std::string& table_name, const std::string& filename)
{
    std::cout << "* Import data (single INSERTs)...\n";

    std::ifstream in{filename};
    int count = 0;
    auto t0 = std::chrono::high_resolution_clock::now();

    std::unique_ptr<sql::Statement> stmt(con->createStatement());

    while (in) {
        std::string s1;  if (!std::getline(in, s1, '\t')) break;
        std::string s2;  if (!std::getline(in, s2, '\t')) break;
        std::string s3;  if (!std::getline(in, s3, '\t')) break;
        std::string s4;  if (!std::getline(in, s4, '\t')) break;
        std::string s5;  if (!std::getline(in, s5, '\t')) break;
        std::string s6;  if (!std::getline(in, s6, '\t')) break;
        std::string s7;  if (!std::getline(in, s7))       break;

        std::string sql{"INSERT INTO " + table_name + " (time, request, duration, user, ip_address, action, user_agent) VALUES " +
                        "(\"" + s1 + "\"," + s2 + "," + s3 + "," + s4 + ",\"" + s5 + "\",\"" + s6 + "\",\"" + s7 + "\")"};

        stmt->execute(sql);
        ++count;
    }

    stmt.release();

    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << "  --> Imported " << count << " rows in " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";
}

void drop_table(sql::Connection* con, const std::string& table_name)
{
    std::cout << "DROP TABLE " << table_name << "...\n";

    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS " + table_name);
}

void create_table(sql::Connection* con, const std::string& table_name)
{
    std::cout << "CREATE TABLE " << table_name << "...\n";

    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    stmt->execute("CREATE TABLE " + table_name + " ("
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

    stmt.release();
    std::this_thread::sleep_for(500ms);
}

std::unique_ptr<sql::Connection> connect(sql::ConnectOptionsMap& connection_properties)
{
    std::cout << "Connecting...\n";

    sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
    std::unique_ptr<sql::Connection> con(driver->connect(connection_properties));
    return con;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
        throw std::runtime_error{"missing performance data filename"};

    const std::string performance_data_filename{argv[1]};

    auto connection_properties = read_mysql_config("mysql_connector_jdbc.json");
    std::unique_ptr<sql::Connection> con{connect(connection_properties)};

    warmup(performance_data_filename);

    drop_table(con.get(), "performance");
    create_table(con.get(), "performance");
    import(con.get(), "performance", performance_data_filename);
}
