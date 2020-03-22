#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace Example {
    struct Ship {
        int id;
        int length;
        int crew;
        std::string affiliation;
        std::string category;
        std::string manufacturer;
        std::string model;
        std::string ship_class;
        std::vector<std::string> roles;
    };

    void to_json(nlohmann::json& j, const Ship& ship)
    {
        j = nlohmann::json{
            {"id", ship.id},
            {"affiliation", ship.affiliation},
            {"category", ship.category},
            {"manufacturer", ship.manufacturer},
            {"model", ship.model},
            {"ship_class", ship.ship_class},
            {"roles", ship.roles},
            {"length", ship.length},
            {"crew", ship.crew}};
    }

    void from_json(const nlohmann::json& j, Ship& ship)
    {
        j.at("id").get_to(ship.id);
        j.at("affiliation").get_to(ship.affiliation);
        j.at("category").get_to(ship.category);
        j.at("manufacturer").get_to(ship.manufacturer);
        j.at("model").get_to(ship.model);
        j.at("ship_class").get_to(ship.ship_class);
        j.at("roles").get_to(ship.roles);
        j.at("length").get_to(ship.length);
        j.at("crew").get_to(ship.crew);
    }
} // namespace Example

int main()
{
    std::ifstream in("ships.json");
    nlohmann::json j;

    in >> j;

    int id = 1;
    std::vector<Example::Ship> ships;

    for (auto& entry : j) {
        entry["id"] = id++;
        auto ship = entry.get<Example::Ship>();

        std::cout << "("
                  << ship.id << ", "
                  << ship.length << ", "
                  << ship.crew << ", "
                  << "\"" << ship.affiliation << "\", "
                  << "\"" << ship.category << "\", "
                  << "\"" << ship.manufacturer << "\", "
                  << "\"" << ship.model << "\", "
                  << "\"" << ship.ship_class << "\"),\n";

        ships.push_back(ship);
    }

    for (const auto& ship : ships)
        for (const auto& role : ship.roles)
            std::cout << "(" << ship.id << ", \"" << role << "\"),\n";
}
