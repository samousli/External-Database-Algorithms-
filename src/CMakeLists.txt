cmake_minimum_required (VERSION 2.8.11)
project (DBI)

set(CMAKE_CXX_FLAGS "-std=c++11 -g -Wall")

file(GLOB SOURCES "*.h")
file(GLOB SOURCES "*.cpp")

add_executable (DBI ${SOURCES})