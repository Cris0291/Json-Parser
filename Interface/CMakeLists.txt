cmake_minimum_required(VERSION 3.30)
project(Interface)

set(CMAKE_CXX_STANDARD 23)

add_executable(Interface main.cpp)

target_link_libraries(Interface PRIVATE Parse)
