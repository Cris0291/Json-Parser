cmake_minimum_required(VERSION 3.30)
project(Parse)

set(CMAKE_CXX_STANDARD 23)

add_library(Parse STATIC
        Tokenizer/Tokenizer.cpp
        Tokenizer/Tokenizer.h
        Tokenizer/TokenState.h
        Tokenizer/TokenNode.h
        Tokenizer/TokenNode.cpp
        Tokenizer/LookUpTable.h
        Deserializer/JsonDescriptor.cpp
        Deserializer/JsonDescriptor.h
        Deserializer/JsonValue.cpp
        Deserializer/JsonValue.h
        Deserializer/DescriptorState.h
)

target_include_directories(Parse PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})