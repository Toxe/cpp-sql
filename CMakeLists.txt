cmake_minimum_required(VERSION 3.14)
project("SQL" LANGUAGES CXX)

# default compiler options and warnings
include(cmake/DefaultCompilerOptionsAndWarnings.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_HOST_UNIX)
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
    find_package(PkgConfig REQUIRED)
endif()

find_library(LibMySQLCppConn_LIBRARY mysqlcppconn)
find_path(LibMySQLCppConn_INCLUDE_DIR mysql/jdbc.h)

find_package(Boost COMPONENTS thread)
find_package(unofficial-iconv CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(sqlpp11 CONFIG REQUIRED)
find_library(sqlpp11_mysql_LIBRARY sqlpp-mysql)

if(CMAKE_HOST_UNIX)
    pkg_check_modules(mariadb REQUIRED libmariadb)
    set(mariadb_LIBRARY "${pkgcfg_lib_mariadb_mariadb}")
endif()

if(CMAKE_HOST_WIN32)
    find_library(mariadb_LIBRARY mariadb)
endif()

set(ALL_TARGETS
    sqlpp11_mysql
    dump_ships_json
)

# mysql_jdbc is optional
if(LibMySQLCppConn_LIBRARY)
    list(APPEND ALL_TARGETS mysql_jdbc)
else()
    message(STATUS "LibMySQLCppConn not found. Skipping building mysql_jdbc.cpp")
endif()

foreach(target ${ALL_TARGETS})
    add_executable(${target} ${target}.cpp)
    target_compile_features(${target} PUBLIC cxx_std_17)
    target_compile_options(${target} PRIVATE ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
endforeach()

if(LibMySQLCppConn_LIBRARY)
    target_include_directories(mysql_jdbc PRIVATE ${LibMySQLCppConn_INCLUDE_DIR})
    target_link_libraries(mysql_jdbc PRIVATE "${LibMySQLCppConn_LIBRARY}")
endif()

target_link_libraries(sqlpp11_mysql PRIVATE sqlpp11 unofficial::iconv::libiconv unofficial::iconv::libcharset Boost::thread nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY})
target_link_libraries(dump_ships_json PRIVATE nlohmann_json::nlohmann_json)
