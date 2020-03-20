#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

constexpr int number_of_values_per_insert = 1000;

void insert_multi(const std::vector<std::string>& lines)
{
    std::string sql{"INSERT INTO performance (time, request, duration, user, ip_address, action, user_agent) VALUES\n" + lines[0]};

    for (auto it = std::next(lines.cbegin()); it != lines.cend(); ++it) {
        sql += ",\n";
        sql += *it;
    }

    std::cout << sql << ";\n";
}

int main(int argc, char* argv[])
{
    if (argc < 2)
        throw std::runtime_error{"missing performance data filename"};

    std::ifstream in{argv[1]};

    if (!in.is_open())
        throw std::runtime_error{"unable to open input file"};

    std::cout << "DROP TABLE IF EXISTS performance;\n"
                 "CREATE TABLE performance (\n"
                 "    id           INT NOT NULL AUTO_INCREMENT,\n"
                 "    time         DATETIME NOT NULL,\n"
                 "    request      INT NOT NULL,\n"
                 "    duration     INT NOT NULL,\n"
                 "    user         INT NOT NULL,\n"
                 "    ip_address   VARCHAR(16) NOT NULL,\n"
                 "    action       VARCHAR(64) NOT NULL,\n"
                 "    user_agent   VARCHAR(255) NOT NULL,\n"
                 "    PRIMARY KEY (id)\n"
                 ") CHARSET=utf8 COLLATE=utf8_unicode_ci;\n";

    std::vector<std::string> lines;

    while (in) {
        std::string s1;  if (!std::getline(in, s1, '\t')) break;
        std::string s2;  if (!std::getline(in, s2, '\t')) break;
        std::string s3;  if (!std::getline(in, s3, '\t')) break;
        std::string s4;  if (!std::getline(in, s4, '\t')) break;
        std::string s5;  if (!std::getline(in, s5, '\t')) break;
        std::string s6;  if (!std::getline(in, s6, '\t')) break;
        std::string s7;  if (!std::getline(in, s7))       break;

        lines.emplace_back("(\"" + s1 + "\"," + s2 + "," + s3 + "," + s4 + ",\"" + s5 + "\",\"" + s6 + "\",\"" + s7 + "\")");

        if (lines.size() == number_of_values_per_insert) {
            insert_multi(lines);
            lines.clear();
        }
    }

    if (lines.size() > 0)
        insert_multi(lines);
}
