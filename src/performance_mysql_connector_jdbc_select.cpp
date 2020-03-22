#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

#include <date.h>
#include <mysql/jdbc.h>
#include <tz.h>

#include "performance_mysql_connector_jdbc_common.h"

void select(sql::Connection* con, const std::string& table_name)
{
    std::cout << "* Query data...\n";

    int count_rows = 0;
    int min_request = std::numeric_limits<int>::max();
    int max_request = std::numeric_limits<int>::min();

    std::map<std::string, std::pair<int, long long>> actions_and_durations{ // action-prefix, count, sum of durations
        {"collection", {0, 0}},
        {"doc", {0, 0}},
        {"file", {0, 0}},
        {"project", {0, 0}},
        {"user", {0, 0}}};

    std::tuple<int, int, std::string, std::string, std::string> longest_request{0, 0, "", "", ""};  // duration, user-id, action, ip address, user-agent

    auto time = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>{};
    auto min_time = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>::max();
    auto max_time = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>::min();

    auto t0 = std::chrono::high_resolution_clock::now();

    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM " + table_name));

    while (res->next()) {
        const std::string action{res->getString("action")};
        const auto dot = action.find('.');

        if (dot == std::string::npos)
            continue;

        const std::string action_prefix{action.substr(0, dot)};

        ++actions_and_durations[action_prefix].first;
        actions_and_durations[action_prefix].second += res->getInt64("duration");

        const int request = res->getInt("request");
        min_request = std::min(min_request, request);
        max_request = std::max(max_request, request);

        std::istringstream{res->getString("time")} >> date::parse("%F %T", time);
        min_time = std::min(min_time, time);
        max_time = std::max(max_time, time);

        if (res->getInt("duration") > std::get<0>(longest_request))
            longest_request = std::make_tuple(res->getInt("duration"), res->getInt("user"), action, res->getString("ip_address"), res->getString("user_agent"));

        ++count_rows;
    }

    res.release();
    stmt.release();

    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "  --> Read " << count_rows << " rows in " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";
    std::cout << "request IDs: min=" << min_request << ", max=" << max_request << '\n';
    std::cout << "time: min=" << date::format("%F %T", min_time) << ", max=" << date::format("%F %T", max_time) << '\n';
    std::cout << "longest request: " << "duration=" << std::get<0>(longest_request) << ", user=" << std::get<1>(longest_request) << ", action=" << std::get<2>(longest_request) << ", ip_address=" << std::get<3>(longest_request) << ", user_agent=" << std::get<4>(longest_request) << '\n';

    for (const auto& [key, value] : actions_and_durations)
        std::cout << key << ": " << value.first << " times, " << value.second << " ms\n";
}

int main()
{
    auto connection_properties = read_mysql_config("mysql_connector_jdbc.json");
    std::unique_ptr<sql::Connection> con{connect(connection_properties)};

    select(con.get(), "performance");
}
