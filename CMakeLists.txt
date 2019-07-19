cmake_minimum_required(VERSION 3.14)
project("SQL" LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_library(LibMySQLCppConn_LIBRARY mysqlcppconn)
find_path(LibMySQLCppConn_INCLUDE_DIR mysql/jdbc.h)

add_executable(mysql_jdbc mysql_jdbc.cpp)
target_compile_features(mysql_jdbc PUBLIC cxx_std_17)
target_compile_options(mysql_jdbc PRIVATE -Wall -Wextra -Wmost -pedantic -Wconversion -Wfloat-equal -Wold-style-cast -fcolor-diagnostics)
target_include_directories(mysql_jdbc PRIVATE ${LibMySQLCppConn_INCLUDE_DIR})
target_link_libraries(mysql_jdbc PRIVATE "${LibMySQLCppConn_LIBRARY}")
set_target_properties(mysql_jdbc PROPERTIES CXX_EXTENSIONS OFF)
