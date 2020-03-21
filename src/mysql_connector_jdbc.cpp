#include <fstream>
#include <iostream>
#include <memory>
#include <mysql/jdbc.h>
#include <nlohmann/json.hpp>

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

int main()
{
    auto connection_properties = read_mysql_config("mysql_connector_jdbc.json");

    sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
    std::unique_ptr<sql::Connection> con(driver->connect(connection_properties));
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM ships ORDER BY category, affiliation, model"));

    std::cout << std::left << std::setw(6) << "id" << std::setw(20) << "category" << std::setw(20) << "affiliation" << "model" << '\n';
    std::cout << "---------------------------------------------------------------------------" << '\n';

    while (res->next()) {
        std::cout << std::left
                  << std::setw(6)  << res->getString("id")
                  << std::setw(20) << res->getString("category")
                  << std::setw(20) << res->getString("affiliation")
                  << res->getString("model") << '\n';
    }
}
