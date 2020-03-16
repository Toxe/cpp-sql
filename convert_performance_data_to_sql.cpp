#include <fstream>
#include <iostream>
#include <string>

int main()
{
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
                 ") CHARSET=utf8 COLLATE=utf8_unicode_ci;\n"
                 "INSERT INTO performance (time, request, duration, user, ip_address, action, user_agent) VALUES\n";

    std::ifstream in{"out.txt"};
    bool add_endline = false;

    while (in) {
        std::string s1;  if (!std::getline(in, s1, '\t')) break;
        std::string s2;  if (!std::getline(in, s2, '\t')) break;
        std::string s3;  if (!std::getline(in, s3, '\t')) break;
        std::string s4;  if (!std::getline(in, s4, '\t')) break;
        std::string s5;  if (!std::getline(in, s5, '\t')) break;
        std::string s6;  if (!std::getline(in, s6, '\t')) break;
        std::string s7;  if (!std::getline(in, s7))       break;

        if (add_endline)
            std::cout << ",\n";
        else
            add_endline = true;

        std::cout << "(\"" << s1 << "\"," << s2 << "," << s3 << "," << s4 << ",\"" << s5 << "\",\"" << s6 << "\",\"" << s7 << "\")";
    }

    std::cout << ";\n";
}
