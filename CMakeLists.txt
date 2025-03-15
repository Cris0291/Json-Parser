cmake_minimum_required(VERSION 3.30)
project(Json_Project)

set(CMAKE_CXX_STANDARD 23)

add_library(Json_Project SHARED
        Tokenizer/Tokenizer.cpp
        Tokenizer/Tokenizer.h
)
