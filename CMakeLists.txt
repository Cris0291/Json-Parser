cmake_minimum_required(VERSION 3.30)
project(Json_Project)

set(CMAKE_CXX_STANDARD 23)

# Add the first child project
add_subdirectory(Parse)

# Add the second child project
add_subdirectory(Interface)