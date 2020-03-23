# all sqlpp11 targets
set(ALL_TARGETS_SQLPP11
    sqlpp11_mysql
    performance_sqlpp11_common
    performance_sqlpp11_insert_multi
    performance_sqlpp11_insert_single
    performance_sqlpp11_select
)

# all MySQL Connector/C++ targets, if they can be built
if(LibMySQLCppConn_LIBRARY AND LibMySQLCppConn_INCLUDE_DIR)
    set(ALL_TARGETS_MYSQL_CONNECTOR_CPP
        mysql_connector_jdbc
        performance_mysql_connector_jdbc_common
        performance_mysql_connector_jdbc_insert_multi
        performance_mysql_connector_jdbc_insert_single
        performance_mysql_connector_jdbc_select
        performance_mysql_connector_jdbc_select_empty
    )
endif()

# all targets: sqlpp11, MySQL Connector/C++ and common ones
set(ALL_TARGETS
    dump_ships_json
    generate_performance_data
    convert_performance_data_to_sql
    ${ALL_TARGETS_SQLPP11}
    ${ALL_TARGETS_MYSQL_CONNECTOR_CPP}
)

# executables and libraries with shared code
add_executable(dump_ships_json dump_ships_json.cpp)
add_executable(generate_performance_data generate_performance_data.cpp)
add_executable(convert_performance_data_to_sql convert_performance_data_to_sql.cpp)
add_executable(sqlpp11_mysql sqlpp11_mysql.cpp include/ships.h)
add_executable(performance_sqlpp11_insert_multi performance_sqlpp11_insert_multi.cpp include/performance.h)
add_executable(performance_sqlpp11_insert_single performance_sqlpp11_insert_single.cpp include/performance.h)
add_executable(performance_sqlpp11_select performance_sqlpp11_select.cpp include/performance.h)

add_library(performance_sqlpp11_common STATIC performance_sqlpp11_common.cpp include/performance_sqlpp11_common.h)

if(ALL_TARGETS_MYSQL_CONNECTOR_CPP)
    add_executable(mysql_connector_jdbc mysql_connector_jdbc.cpp)
    add_executable(performance_mysql_connector_jdbc_insert_multi performance_mysql_connector_jdbc_insert_multi.cpp)
    add_executable(performance_mysql_connector_jdbc_insert_single performance_mysql_connector_jdbc_insert_single.cpp)
    add_executable(performance_mysql_connector_jdbc_select performance_mysql_connector_jdbc_select.cpp)
    add_executable(performance_mysql_connector_jdbc_select_empty performance_mysql_connector_jdbc_select_empty.cpp)

    add_library(performance_mysql_connector_jdbc_common STATIC performance_mysql_connector_jdbc_common.cpp include/performance_mysql_connector_jdbc_common.h)
endif()

# common options for all targets
foreach(target ${ALL_TARGETS})
    target_compile_features(${target} PUBLIC cxx_std_17)
    target_compile_options(${target} PRIVATE ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
endforeach()

get_filename_component(date_INCLUDE_DIR ${HinnantDate_INCLUDE_FILE} DIRECTORY)

# options for common targets
target_include_directories(generate_performance_data PRIVATE ${date_INCLUDE_DIR})

target_link_libraries(dump_ships_json PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(generate_performance_data PRIVATE date::date date::tz)

# options for sqlpp11 targets
target_include_directories(performance_sqlpp11_common PUBLIC include)

target_include_directories(sqlpp11_mysql PRIVATE include ${date_INCLUDE_DIR})
target_include_directories(performance_sqlpp11_insert_multi PRIVATE include ${date_INCLUDE_DIR})
target_include_directories(performance_sqlpp11_insert_single PRIVATE include ${date_INCLUDE_DIR})
target_include_directories(performance_sqlpp11_select PRIVATE include ${date_INCLUDE_DIR})

if(CMAKE_HOST_WIN32)
    # Windows
    target_link_libraries(performance_sqlpp11_common PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY})

    target_link_libraries(sqlpp11_mysql PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY})
    target_link_libraries(performance_sqlpp11_insert_multi PRIVATE performance_sqlpp11_common date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY})
    target_link_libraries(performance_sqlpp11_insert_single PRIVATE performance_sqlpp11_common date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY})
    target_link_libraries(performance_sqlpp11_select PRIVATE performance_sqlpp11_common date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY})
elseif(CMAKE_HOST_APPLE)
    # macOS
    target_link_libraries(performance_sqlpp11_common PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} unofficial::iconv::libiconv unofficial::iconv::libcharset)

    target_link_libraries(sqlpp11_mysql PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} unofficial::iconv::libiconv unofficial::iconv::libcharset)
    target_link_libraries(performance_sqlpp11_insert_multi PRIVATE performance_sqlpp11_common date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} unofficial::iconv::libiconv unofficial::iconv::libcharset)
    target_link_libraries(performance_sqlpp11_insert_single PRIVATE performance_sqlpp11_common date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} unofficial::iconv::libiconv unofficial::iconv::libcharset)
    target_link_libraries(performance_sqlpp11_select PRIVATE performance_sqlpp11_common date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} unofficial::iconv::libiconv unofficial::iconv::libcharset)
else()
    # Linux
    target_link_libraries(performance_sqlpp11_common PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})

    target_link_libraries(sqlpp11_mysql PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})
    target_link_libraries(performance_sqlpp11_insert_multi PRIVATE performance_sqlpp11_common date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})
    target_link_libraries(performance_sqlpp11_insert_single PRIVATE performance_sqlpp11_common date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})
    target_link_libraries(performance_sqlpp11_select PRIVATE performance_sqlpp11_common date::date date::tz ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})
endif()

# options for MySQL Connector/C++ targets
if(ALL_TARGETS_MYSQL_CONNECTOR_CPP)
    target_include_directories(performance_mysql_connector_jdbc_common PUBLIC include PRIVATE ${LibMySQLCppConn_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

    target_include_directories(mysql_connector_jdbc PRIVATE ${LibMySQLCppConn_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
    target_include_directories(performance_mysql_connector_jdbc_insert_multi PRIVATE ${LibMySQLCppConn_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
    target_include_directories(performance_mysql_connector_jdbc_insert_single PRIVATE ${LibMySQLCppConn_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
    target_include_directories(performance_mysql_connector_jdbc_select PRIVATE ${LibMySQLCppConn_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${date_INCLUDE_DIR})
    target_include_directories(performance_mysql_connector_jdbc_select_empty PRIVATE ${LibMySQLCppConn_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${date_INCLUDE_DIR})

    target_link_libraries(performance_mysql_connector_jdbc_common PRIVATE nlohmann_json::nlohmann_json ${LibMySQLCppConn_LIBRARY} ${libresolv})

    target_link_libraries(mysql_connector_jdbc PRIVATE nlohmann_json::nlohmann_json ${LibMySQLCppConn_LIBRARY} ${libresolv})
    target_link_libraries(performance_mysql_connector_jdbc_insert_multi PRIVATE performance_mysql_connector_jdbc_common ${LibMySQLCppConn_LIBRARY} ${libresolv})
    target_link_libraries(performance_mysql_connector_jdbc_insert_single PRIVATE performance_mysql_connector_jdbc_common ${LibMySQLCppConn_LIBRARY} ${libresolv})
    target_link_libraries(performance_mysql_connector_jdbc_select PRIVATE performance_mysql_connector_jdbc_common date::date date::tz ${LibMySQLCppConn_LIBRARY} ${libresolv})
endif()
