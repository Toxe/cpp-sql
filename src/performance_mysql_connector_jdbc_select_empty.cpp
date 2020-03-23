#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include <mysql/jdbc.h>

#include "performance_mysql_connector_jdbc_common.h"

void select(sql::Connection* con, const std::string& table_name)
{
    std::cout << "* Query data...\n";

    int count_rows = 0;
    auto t0 = std::chrono::high_resolution_clock::now();

    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM " + table_name));

    while (res->next())
        ++count_rows;

    res.release();
    stmt.release();

    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "  --> Read " << count_rows << " rows in " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";
}

int main()
{
    auto connection_properties = read_mysql_config("mysql_connector_jdbc.json");
    std::unique_ptr<sql::Connection> con{connect(connection_properties)};

    select(con.get(), "performance");
}
