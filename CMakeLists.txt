cmake_minimum_required(VERSION 3.13)
project("SQL" LANGUAGES CXX)

# default compiler options and warnings
include(cmake/DefaultCompilerOptionsAndWarnings.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_HOST_UNIX)
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
    find_package(PkgConfig REQUIRED)
endif()

find_package(nlohmann_json CONFIG REQUIRED)
find_package(date CONFIG REQUIRED)
find_package(Threads REQUIRED)

# the MySQL Connector/C++ targets are optional
find_library(LibMySQLCppConn_LIBRARY mysqlcppconn-static PATHS ${MYSQL_CONNECTOR_CPP_DIR}/lib DOC "MySQL Connector/C++ library")
find_path(LibMySQLCppConn_INCLUDE_DIR mysql/jdbc.h PATHS ${MYSQL_CONNECTOR_CPP_DIR}/include DOC "MySQL Connector/C++ headers")

if(LibMySQLCppConn_LIBRARY AND LibMySQLCppConn_INCLUDE_DIR)
    message(STATUS "Found MySQL Connector/C++: " ${LibMySQLCppConn_LIBRARY})
    find_package(Boost REQUIRED)

    if(CMAKE_HOST_APPLE)
        find_library(libresolv resolv)
    endif()
else()
    message(STATUS "MySQL Connector/C++ not found, skipping building of mysql_connector_jdbc.cpp. Set MYSQL_CONNECTOR_CPP_DIR to MySQL Connector/C++ installation directory.")
endif()

# for sqlpp11 targets
find_package(Sqlpp11 CONFIG REQUIRED)
find_library(sqlpp11_mysql_LIBRARY sqlpp-mysql)

if(CMAKE_HOST_UNIX)
    # macOS and Linux
    pkg_check_modules(mariadb REQUIRED libmariadb)
    set(mariadb_LIBRARY "${pkgcfg_lib_mariadb_mariadb}")

    if(CMAKE_HOST_APPLE)
        # macOS only
        find_package(unofficial-iconv CONFIG REQUIRED)
    endif()
endif()

if(CMAKE_HOST_WIN32)
    # Windows
    find_library(mariadb_LIBRARY libmariadb)
endif()

add_subdirectory(src)
