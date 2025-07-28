cmake_minimum_required(VERSION 3.30)
project(Interface)

set(CMAKE_CXX_STANDARD 23)

add_executable(Interface main.cpp)

target_include_directories(Interface
        PRIVATE
        ${CMAKE_SOURCE_DIR}/Parse/Tokenizer
        ${CMAKE_SOURCE_DIR}/Parse/Deserializer
)
target_link_libraries(Interface
        PRIVATE
        Parse
)
