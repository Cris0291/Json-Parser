//
// Created by cris0 on 5/2/2025.
//

#ifndef JSONOBJECT_H
#define JSONOBJECT_H
#include <string>
#include <unordered_map>
#include <vector>

#include "JsonValue.h"
#include "Tokenizer/TokenState.h"


class JsonDescriptor {
public:
    template<typename T>
    T get(const std::string& key) const;
private:
    std::unordered_map<std::string, JsonValue> json_map;
    void createJsonMap(const std::vector<State::Token>& tokens);
    template<typename  T>
    static T parseJsonToken(const std::string& value);
};

template<typename T>
T from_json(const JsonDescriptor& obj);

enum class FieldType {
    Int,
    String,
    Boolean,
    Double,
    Array,
    Vector,
    CArray,
    Pointer,
    DoublePointer
};
template<typename T>
constexpr FieldType getType();

template<typename T, template<typename...> class Template>
struct is_specialization_of : std::false_type {};

template<template<typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

template<typename T>
struct is_array_specialization : std::false_type {};

template<typename T, std::size_t N>
struct is_array_specialization<std::array<T,N>> : std::true_type {};

template<typename T>
struct is_c_array : std::false_type {};

template<typename T, std::size_t N>
struct is_c_array<T[N]> : std::true_type {};

template<typename T>
struct is_pointer: std::false_type {};

template<typename T>
struct is_pointer<T*> : std::true_type {};

template<typename T>
struct is_double_pointer: std::false_type {};

template<typename T>
struct is_double_pointer<T**> : std::true_type {};
#endif //JSONOBJECT_H
