//
// Created by cris0 on 5/2/2025.
//

#ifndef JSONOBJECT_H
#define JSONOBJECT_H
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <forward_list>
#include <optional>

#include "Deserializer/JsonValue.h"
#include "Tokenizer/TokenState.h"

enum class FieldType {
    Int,
    String,
    Boolean,
    Double,
    Array,
    Vector,
    CArray,
    Pointer,
    DoublePointer,
    Object
};

class JsonDescriptor {
public:
    explicit JsonDescriptor(std::unordered_map<std::string, JsonValue> map);
    JsonValue get(const std::string& key) const;
private:
    std::unordered_map<std::string, JsonValue> json_map;
    void createJsonMap(const std::vector<State::Token>& tokens);
    template<typename  T>
    static T parseJsonToken(const std::string& value);
};

template<typename T>
T from_json(const JsonDescriptor& obj);

template<typename T>
constexpr FieldType getType();

template<typename T>
concept Iterable = requires(T t)
{
  {std::begin(t)} -> std::input_iterator;
    {std::end(t)} -> std::sentinel_for<decltype(std::begin(t))>;
};

template<typename T, template<typename...> class Template>
struct is_specialization_of : std::false_type {};

template<template<typename...> class Template, typename... Args>
struct is_specialization_of<Template<Args...>, Template> : std::true_type {};

template<typename T, template<typename...> class Template>
concept specialization_of = is_specialization_of<T, Template>::value;

template<typename T, template<typename...> class...Templates>
concept specialization_of_any = (specialization_of<T, Templates> || ...);

template<typename T>
concept Containerable = specialization_of_any<T, std::vector, std::list, std::forward_list>;

template<typename T>
struct is_array_specialization : std::false_type {};

template<typename T, std::size_t N>
struct is_array_specialization<std::array<T,N>> : std::true_type {};

template<typename T>
concept specialization_of_array = is_array_specialization<T>::value;

template<typename T>
struct is_c_array : std::false_type {};

template<typename T, std::size_t N>
struct is_c_array<T[N]> : std::true_type {};

template<typename T>
concept specialization_of_c_array = is_c_array<T>::value;

template<typename T>
struct is_pointer: std::false_type {};

template<typename T>
struct is_pointer<T*> : std::true_type {};

template<typename T>
concept  specialization_of_pointer = is_pointer<T>::value;

template<typename T>
concept NotAContainerOrArray =
    (!Containerable<T>)
&& (!specialization_of_array<T>)
&& (!specialization_of_c_array<T>);

template<typename T>
concept  PointerToLeaf =
    specialization_of_pointer<T> && NotAContainerOrArray<std::remove_pointer_t<T>>;

template<typename T>
struct is_double_pointer: std::false_type {};

template<typename T>
struct is_double_pointer<T**> : std::true_type {};

template<typename T>
struct is_optional : std::false_type {};

template<typename T>
struct  is_optional<std::optional<T>> : std::true_type {};

template<typename T>
concept Optional = is_optional<T>::value;
#endif //JSONOBJECT_H
