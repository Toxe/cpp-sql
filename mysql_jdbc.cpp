#include <iostream>
#include <memory>
#include <mysql/jdbc.h>

int main()
{
    sql::ConnectOptionsMap connection_properties;
    connection_properties["readDefaultFile"] = "~/.my.cnf";
    connection_properties["socket"]          = "/Applications/MAMP/tmp/mysql/mysql.sock";

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
