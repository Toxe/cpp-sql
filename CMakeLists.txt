cmake_minimum_required(VERSION 3.14)
project("SQL" LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(LibArchive)

# Theoretisch sollte es reichen CMake mit -DCMAKE_PREFIX_PATH=$SUITE_BUILD aufzurufen,
# aber aus irgendeinem Grund wird dann die System Version von libarchive.dylib verwendet.
include_directories("$ENV{SUITE_BUILD}/include")

set(LibMySQLCppConn_LIBRARY "$ENV{SUITE_BUILD}/lib64/libmysqlcppconn.dylib")

add_executable(mysql_jdbc mysql_jdbc.cpp)
target_compile_features(mysql_jdbc PUBLIC cxx_std_17)
target_link_libraries(mysql_jdbc PRIVATE "${LibMySQLCppConn_LIBRARY}")
set_target_properties(mysql_jdbc PROPERTIES CXX_EXTENSIONS OFF)
