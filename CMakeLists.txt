cmake_minimum_required(VERSION 3.16)

project(json)

set(CMAKE_CXX_STANDARD 20)

add_executable(json main.cpp)

target_sources(json PRIVATE "main.cpp" "json.cpp" "token.cpp")
