cmake_minimum_required(VERSION 3.14)

project(swss)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_C_FLAGS_DEBUG "-g")

find_package(libwebsockets REQUIRED)

add_executable(server server.cpp)
target_link_libraries(server websockets)

add_executable(client client.cpp)
target_link_libraries(client websockets)