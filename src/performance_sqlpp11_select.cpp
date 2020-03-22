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
#include <sqlpp11/mysql/mysql.h>
#include <sqlpp11/sqlpp11.h>
#include <tz.h>

#include "performance.h"
#include "performance_sqlpp11_common.h"

void select(sqlpp::mysql::connection& db)
{
    std::cout << "* Query data...\n";

    int count_rows = 0;
    int64_t min_request = std::numeric_limits<int64_t>::max();
    int64_t max_request = std::numeric_limits<int64_t>::min();

    std::map<std::string, std::pair<int, long long>> actions_and_durations{ // action-prefix, count, sum of durations
        {"collection", {0, 0}},
        {"doc", {0, 0}},
        {"file", {0, 0}},
        {"project", {0, 0}},
        {"user", {0, 0}}};

    std::tuple<int64_t, int64_t, std::string, std::string, std::string> longest_request{0, 0, "", "", ""};  // duration, user-id, action, ip address, user-agent

    auto time = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>{};
    auto min_time = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>::max();
    auto max_time = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>::min();

    auto t0 = std::chrono::high_resolution_clock::now();

    Performance::Performance performance{};

    for (const auto& row : db(select(all_of(performance)).from(performance).unconditionally())) {
        const std::string action{row.action};
        const auto dot = action.find('.');

        if (dot == std::string::npos)
            continue;

        const std::string action_prefix{action.substr(0, dot)};

        ++actions_and_durations[action_prefix].first;
        actions_and_durations[action_prefix].second += row.duration;

        min_request = std::min<int64_t>(min_request, row.request);
        max_request = std::max<int64_t>(max_request, row.request);

        time = row.time;
        min_time = std::min(min_time, time);
        max_time = std::max(max_time, time);

        if (row.duration > std::get<0>(longest_request))
            longest_request = std::make_tuple<int64_t, int64_t, std::string, std::string, std::string>(row.duration, row.user, row.action, row.ipAddress, row.userAgent);

        ++count_rows;
    }

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
    auto config = read_mysql_config("sqlpp11_mysql.json");
    sqlpp::mysql::connection db(config);

    select(db);
}
