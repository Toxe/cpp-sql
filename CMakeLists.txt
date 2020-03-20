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

# for mysql_connector_jdbc.cpp
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

# for sqlpp11 examples
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

set(ALL_TARGETS
    sqlpp11_mysql
    performance_sqlpp11_insert_single
    dump_ships_json
    generate_performance_data
    convert_performance_data_to_sql
)

# mysql_connector_jdbc is optional
if(LibMySQLCppConn_LIBRARY AND LibMySQLCppConn_INCLUDE_DIR)
    list(APPEND ALL_TARGETS
         mysql_connector_jdbc
         performance_mysql_connector_jdbc_insert_multi
         performance_mysql_connector_jdbc_insert_single
    )
endif()

foreach(target ${ALL_TARGETS})
    add_executable(${target} ${target}.cpp)
    target_compile_features(${target} PUBLIC cxx_std_17)
    target_compile_options(${target} PRIVATE ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
endforeach()

if(LibMySQLCppConn_LIBRARY)
    target_include_directories(mysql_connector_jdbc PRIVATE ${LibMySQLCppConn_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
    target_include_directories(performance_mysql_connector_jdbc_insert_multi PRIVATE ${LibMySQLCppConn_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
    target_include_directories(performance_mysql_connector_jdbc_insert_single PRIVATE ${LibMySQLCppConn_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

    target_link_libraries(mysql_connector_jdbc PRIVATE nlohmann_json::nlohmann_json ${LibMySQLCppConn_LIBRARY} ${libresolv})
    target_link_libraries(performance_mysql_connector_jdbc_insert_multi PRIVATE nlohmann_json::nlohmann_json ${LibMySQLCppConn_LIBRARY} ${libresolv})
    target_link_libraries(performance_mysql_connector_jdbc_insert_single PRIVATE nlohmann_json::nlohmann_json ${LibMySQLCppConn_LIBRARY} ${libresolv})
endif()

get_filename_component(date_INCLUDE_DIR ${HinnantDate_INCLUDE_FILE} DIRECTORY)

target_include_directories(generate_performance_data PRIVATE ${date_INCLUDE_DIR})
target_include_directories(performance_sqlpp11_insert_single PRIVATE ${date_INCLUDE_DIR})

target_link_libraries(dump_ships_json PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(generate_performance_data PRIVATE date::date date::tz)

if(CMAKE_HOST_WIN32)
    # Windows
    target_link_libraries(sqlpp11_mysql PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY})
    target_link_libraries(performance_sqlpp11_insert_single PRIVATE nlohmann_json::nlohmann_json date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY})
elseif(CMAKE_HOST_APPLE)
    # macOS
    target_link_libraries(sqlpp11_mysql PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} unofficial::iconv::libiconv unofficial::iconv::libcharset)
    target_link_libraries(performance_sqlpp11_insert_single PRIVATE nlohmann_json::nlohmann_json date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} unofficial::iconv::libiconv unofficial::iconv::libcharset)
else()
    # Linux
    target_link_libraries(sqlpp11_mysql PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})
    target_link_libraries(performance_sqlpp11_insert_single PRIVATE nlohmann_json::nlohmann_json date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})
endif()
