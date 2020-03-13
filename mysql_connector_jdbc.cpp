#include <fstream>
#include <iostream>
#include <memory>
#include <mysql/jdbc.h>
#include <nlohmann/json.hpp>

// Read JSON config file.
//
// Example "mysql_conf.json":
//
//   {
//       "hostName": "localhost",
//       "userName": "username",
//       "password": "password",
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

    stmt->execute("USE contiview");
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM content_object ORDER BY id LIMIT 10"));

    std::cout << "id" << "\t" << "parent" << "\t" << "type" << std::endl;
    std::cout << "----------------------------" << std::endl;

    while (res->next()) {
        std::cout << res->getString("id")     << "\t"
                  << res->getString("parent") << "\t"
                  << res->getString("type")   << std::endl;
    }
}
