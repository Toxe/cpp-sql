#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <mysql/jdbc.h>

#include "performance_mysql_connector_jdbc_common.h"

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
