#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <date.h>
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include <tz.h>

#include "performance.h"
#include "performance_sqlpp11_common.h"

void import(sqlpp::mysql::connection& db, const std::string& filename)
{
    std::cout << "* Import data (single INSERTs)...\n";

    std::ifstream in{filename};
    int count = 0;
    auto time = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>{};
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
    import(db, performance_data_filename);
}
