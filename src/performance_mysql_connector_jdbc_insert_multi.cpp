#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <mysql/jdbc.h>

#include "performance_mysql_connector_jdbc_common.h"

constexpr int number_of_values_per_insert = 1000;

void insert_multi(sql::Statement* stmt, const std::string& table_name, const std::vector<std::string> values)
{
    std::string sql{"INSERT INTO " + table_name + " (time, request, duration, user, ip_address, action, user_agent) VALUES " + values[0]};

    for (auto it = std::next(values.cbegin()); it != values.cend(); ++it) {
        sql += ",\n";
        sql += *it;
    }

    stmt->execute(sql);
}

void import(sql::Connection* con, const std::string& table_name, const std::string& filename)
{
    std::cout << "* Import data (combined INSERTs)...\n";

    std::vector<std::string> values;
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

        values.emplace_back("(\"" + s1 + "\"," + s2 + "," + s3 + "," + s4 + ",\"" + s5 + "\",\"" + s6 + "\",\"" + s7 + "\")");

        if (values.size() == number_of_values_per_insert) {
            insert_multi(stmt.get(), table_name, values);
            count += values.size();
            values.clear();
        }
    }

    if (values.size() > 0) {
        insert_multi(stmt.get(), table_name, values);
        count += values.size();
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
