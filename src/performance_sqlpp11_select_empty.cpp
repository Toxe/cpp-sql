#include <chrono>
#include <iostream>
#include <string>

#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>

#include "performance.h"
#include "performance_sqlpp11_common.h"

void select(sqlpp::mysql::connection& db)
{
    std::cout << "* Query data...\n";

    int count_rows = 0;
    auto t0 = std::chrono::high_resolution_clock::now();

    Performance::Performance performance{};

    for (const auto& row : db(select(all_of(performance)).from(performance).unconditionally()))
        ++count_rows;

    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "  --> Read " << count_rows << " rows in " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";
}

int main()
{
    auto config = read_mysql_config("sqlpp11_mysql.json");
    sqlpp::mysql::connection db(config);

    select(db);
}
