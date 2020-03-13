cmake_minimum_required(VERSION 3.13)
project("SQL" LANGUAGES CXX)

# default compiler options and warnings
include(cmake/DefaultCompilerOptionsAndWarnings.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_HOST_UNIX)
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
    find_package(PkgConfig REQUIRED)
endif()

# for mysql_connector_jdbc.cpp (which is optional)
find_library(LibMySQLCppConn_LIBRARY mysqlcppconn)
find_path(LibMySQLCppConn_INCLUDE_DIR mysql/jdbc.h)

# for sqlpp11 examples
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Sqlpp11 CONFIG REQUIRED)
find_library(sqlpp11_mysql_LIBRARY sqlpp-mysql)

if(CMAKE_HOST_UNIX)
    # macOS and Linux
    pkg_check_modules(mariadb REQUIRED libmariadb)
    set(mariadb_LIBRARY "${pkgcfg_lib_mariadb_mariadb}")

    if(NOT CMAKE_HOST_APPLE)
        # Linux only
        find_package(Threads REQUIRED)
    endif()

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
    dump_ships_json
)

# mysql_connector_jdbc is optional
if(LibMySQLCppConn_LIBRARY)
    list(APPEND ALL_TARGETS mysql_connector_jdbc)
else()
    message(STATUS "LibMySQLCppConn not found. Skipping building mysql_connector_jdbc.cpp")
endif()

foreach(target ${ALL_TARGETS})
    add_executable(${target} ${target}.cpp)
    target_compile_features(${target} PUBLIC cxx_std_17)
    target_compile_options(${target} PRIVATE ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
endforeach()

if(LibMySQLCppConn_LIBRARY)
    target_include_directories(mysql_connector_jdbc PRIVATE ${LibMySQLCppConn_INCLUDE_DIR})
    target_link_libraries(mysql_connector_jdbc PRIVATE "${LibMySQLCppConn_LIBRARY}")
endif()

target_link_libraries(dump_ships_json PRIVATE nlohmann_json::nlohmann_json)

if(CMAKE_HOST_WIN32)
    # Windows
    target_link_libraries(sqlpp11_mysql PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY})
elseif(CMAKE_HOST_APPLE)
    # macOS
    target_link_libraries(sqlpp11_mysql PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} unofficial::iconv::libiconv unofficial::iconv::libcharset)
else()
    # Linux
    target_link_libraries(sqlpp11_mysql PRIVATE nlohmann_json::nlohmann_json ${sqlpp11_mysql_LIBRARY} ${mariadb_LIBRARY} Threads::Threads ${CMAKE_DL_LIBS})
endif()
