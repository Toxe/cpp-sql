#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/mysql/mysql.h>
#include "ships.h"

// Read JSON config file.
//
// Example "sqlpp11_mysql.json":
//
//   {
//       "user": "username",
//       "password": "password",
//       "database": "ships",
//       "unix_socket": "/tmp/mysql.sock"
//   }
std::shared_ptr<sqlpp::mysql::connection_config> read_mysql_config(const char* filename)
{
    std::ifstream in(filename);
    nlohmann::json data;

    in >> data;
    auto config = std::make_shared<sqlpp::mysql::connection_config>();

    if (!data["host"].empty()) config->host = data["host"].get<std::string>();
    if (!data["user"].empty()) config->user = data["user"].get<std::string>();
    if (!data["password"].empty()) config->password = data["password"].get<std::string>();
    if (!data["database"].empty()) config->database = data["database"].get<std::string>();
    if (!data["unix_socket"].empty()) config->unix_socket = data["unix_socket"].get<std::string>();
    if (!data["charset"].empty()) config->charset = data["charset"].get<std::string>();
    if (!data["port"].empty()) config->port = data["port"].get<unsigned int>();
    if (!data["client_flag"].empty()) config->client_flag = data["client_flag"].get<unsigned long>();
    if (!data["auto_reconnect"].empty()) config->auto_reconnect = data["auto_reconnect"].get<bool>();
    if (!data["debug"].empty()) config->debug = data["debug"].get<bool>();

    return config;
}

// these need to be declared at global scope
SQLPP_ALIAS_PROVIDER(col_count)

int main()
{
    auto config = read_mysql_config("sqlpp11_mysql.json");
    sqlpp::mysql::connection db(config);

    Ships::Ships ships{};
    Ships::ShipRoles shipRoles{};

    std::cout << "\n----------------------------------------------------------------------------\n"
              << "SELECT * FROM ships"
              << "\n----------------------------------------------------------------------------\n";

    for (const auto& row : db(select(all_of(ships)).from(ships).unconditionally())) {
        std::cout << row.id << ": " << row.length << ", " << row.crew << ", " << row.affiliation << ", " << row.category << ", " << row.manufacturer << ", " << row.model << ", " << row.shipClass << '\n';
    }

    std::cout << "\n----------------------------------------------------------------------------\n"
              << "SELECT id, model FROM ships ORDER BY model DESC LIMIT 5"
              << "\n----------------------------------------------------------------------------\n";

    for (const auto& row : db(select(ships.id, ships.model).from(ships).unconditionally().order_by(ships.model.desc()).limit(5u))) {
        std::cout << row.id << ": " << row.model << '\n';
    }

    std::cout << "\n----------------------------------------------------------------------------\n"
              << "SELECT id, category, model FROM ships WHERE category = 'Starfighters'"
              << "\n----------------------------------------------------------------------------\n";

    for (const auto& row : db(select(ships.id, ships.category, ships.model).from(ships).where(ships.category == "Starfighters"))) {
        std::cout << row.id << ": " << row.category << ", " << row.model << '\n';
    }

    std::cout << "\n----------------------------------------------------------------------------\n"
              << "SELECT id, model, length FROM ships WHERE length >= 1000 AND model LIKE '%Destroyer%'"
              << "\n----------------------------------------------------------------------------\n";

    for (const auto& row : db(select(ships.id, ships.model, ships.length).from(ships).where(ships.length >= 1000 && ships.model.like("%Destroyer%")))) {
        std::cout << row.id << ": " << row.model << " = " << row.length << '\n';
    }

    std::cout << "\n----------------------------------------------------------------------------\n"
              << "SELECT id, model, length, length * 3.2808 AS feet FROM ships WHERE (length * 3.2808) <= 100 ORDER BY feet DESC"  // it is not allowed to use an alias in WHERE
              << "\n----------------------------------------------------------------------------\n";

    const auto feet = ships.length * 3.2808;

    for (const auto& row : db(select(ships.id, ships.model, ships.length, feet.as(sqlpp::alias::f)).from(ships).where(feet <= 100).order_by(feet.desc()))) {
        std::cout << row.id << ": " << row.model << " = " << row.length << " meters (or " << row.f << " feet)\n";
    }

    std::cout << "\n----------------------------------------------------------------------------\n"
              << "SELECT DISTINCT category FROM ships ORDER BY category ASC"
              << "\n----------------------------------------------------------------------------\n";

    for (const auto& row : db(select(ships.category).flags(sqlpp::distinct).from(ships).unconditionally().order_by(ships.category.asc()))) {
        std::cout << row.category << '\n';
    }

    std::cout << "\n----------------------------------------------------------------------------\n"
              << "SELECT ship_class, COUNT(*) AS col_count FROM ships GROUP BY ship_class HAVING col_count > 1 ORDER BY col_count DESC"
              << "\n----------------------------------------------------------------------------\n";

    const auto ships_count = count(ships.shipClass);

    for (const auto& row : db(select(ships.shipClass, ships_count.as(col_count)).from(ships).unconditionally().group_by(ships.shipClass).having(ships_count > 1).order_by(ships_count.desc()))) {
        std::cout << row.shipClass << ": " << row.col_count << '\n';
    }

    std::cout << "----------------------------------------------------------------------------\n";

    // longer alternative that does exactly the same
    for (const auto& row : db(select(ships.shipClass, count(ships.shipClass).as(col_count)).from(ships).unconditionally().group_by(ships.shipClass).having(count(ships.shipClass) > 1).order_by(count(ships.shipClass).desc()))) {
        std::cout << row.shipClass << ": " << row.col_count << '\n';
    }

    std::cout << "\n----------------------------------------------------------------------------\n"
              << "SELECT ships.id, ships.model, ship_roles.role FROM ships JOIN ship_roles ON ships.id = ship_roles.id ORDER BY id ASC, ship_roles.role ASC LIMIT 10"
              << "\n----------------------------------------------------------------------------\n";

    for (const auto& row : db(select(ships.id, ships.model, shipRoles.role).from(ships.join(shipRoles).on(ships.id == shipRoles.id)).unconditionally().order_by(ships.id.asc(), shipRoles.role.asc()).limit(10u))) {
        std::cout << row.id << ": " << row.model << " = " << row.role << '\n';
    }
}
