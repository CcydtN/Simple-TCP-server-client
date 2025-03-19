cmake_minimum_required(VERSION 3.31)
project(simple_tcp_server_client)

add_definitions(-std=c++20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SERVER_SOURCES src/server.cpp)
add_executable(server ${SERVER_SOURCES})

set(CLIENT_SOURCES src/client.cpp)
add_executable(client ${CLIENT_SOURCES})
