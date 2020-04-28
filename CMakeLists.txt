cmake_minimum_required(VERSION 3.16)
project(bloom_filter)

set(CMAKE_CXX_STANDARD 14)

add_executable(bloom_filter main.cpp )

add_executable(bf_client bf_client.cpp)
add_executable(bf_server bf_server.cpp)