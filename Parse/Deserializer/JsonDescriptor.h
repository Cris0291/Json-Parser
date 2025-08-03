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
#include <filesystem>

#include "JsonValue.h"
#include "TokenState.h"

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
    JsonDescriptor() = default;
    explicit JsonDescriptor(std::unordered_map<std::string, JsonValue> map);
    JsonValue get(const std::string& key) const;
    void createJsonMap(const std::vector<State::Token>& tokens);
    void DisplayMap();
private:
    std::unordered_map<std::string, JsonValue> json_map;
    template<typename  T>
    static T parseJsonToken(const std::string& value);
};

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

template<typename T>
requires requires(T &inst)
{
    {
        deserialize(inst, [](auto const &key, auto &field){})
    } -> std::same_as<void>;
}
T from_json(const JsonDescriptor &obj) {
    T instance {};

    auto binder = [&obj](const std::string& key, auto& field) {
        //using raw_type = decltype(field);
        //using field_type = std::remove_cvref_t<raw_type>;
        const JsonValue& jv = obj.get(key);
        from_json(field, jv);
    };

    deserialize(instance, binder);

    return instance;
}

template<typename T>
requires (!Containerable<T> && !specialization_of_array<T> && !specialization_of_c_array<T> && !PointerToLeaf<T> && !std::is_class_v<T>)
void from_json(T& out, const JsonValue& jv) {
    if (jv.get_null()) {
        throw std::runtime_error("Null values should be wrapped in std::optional");
    }
    if constexpr (std::is_same_v<T, int>) {
        out = jv.get_value_by_index<int, 1>();
    }
    else if constexpr (std::is_same_v<T, double>) {
        out = jv.get_value_by_index<double, 2>();
    }
    else if constexpr (std::is_same_v<T, bool>) {
        out = jv.get_value_by_index<bool, 3>();
    }
}

template<typename T>
requires std::is_same_v<T, std::string>
void from_json(T& out, const JsonValue& jv) {
    if (jv.get_null()) {
        out.clear();
    } else {
        out = jv.get_value_by_index<std::string,4>();
    }
}

template<Containerable C>
void from_json(C& out, const JsonValue& jv) {
    if (jv.get_null()) {
        throw std::runtime_error("Null values should be wrapped in std::optional");
    }
    using inner_elem = typename C::value_type;
    const auto& arr = jv.get_value_by_index<JsonArray, 6>();

    out.clear();
    if constexpr (requires(C& c, std::size_t n) {c.reseve(n);}) {
        out.reseve(arr.size());
    }

    for (const auto& a : arr) {
        inner_elem elem;
        from_json(elem, a);
        out.insert(out.end(), std::move(elem));
    }
}

template<typename A>
requires specialization_of_array<A>
void from_json(A& out, const JsonValue& jv) {
    if (jv.get_null()) {
        throw std::runtime_error("Null values should be wrapped in std::optional");
    }
    //using inner_elem = typename A::value_type;
    constexpr std::size_t N = std::tuple_size_v<A>;

    const auto& arr = jv.get_value_by_index<JsonArray, 6>();
    if (arr.size() != N) {
        throw std::runtime_error(
        "JSON array size mismatch for std::array<…> expected "
        + std::to_string(N)
        + ", but got"
        + std::to_string(arr.size())
            );
    }

    for (std::size_t i = 0; i < N; i++) {
        from_json(out[i], arr[i]);
    }
}

template<typename C>
requires specialization_of_c_array<C>
void from_json(C& out, const JsonValue& jv) {
    if (jv.get_null()) {
        return;
    }
    //using inner_elem = std::remove_extent_t<C>;
    constexpr std::size_t N = std::extent_v<C>;

    const auto& arr = jv.get_value_by_index<JsonArray, 6>();
    if (arr.size() != N) {
        throw std::runtime_error(
        "JSON array size mismatch for T[N] expected "
     + std::to_string(N)
    + ", but got"
    + std::to_string(arr.size())
            );
    }

    for (std::size_t i = 0; i < N; i++) {
        from_json(out[i], arr[i]);
    }
}

template<PointerToLeaf P>
void from_json(P& out, const JsonValue& jv) {
    if (jv.get_null()) {
        out = nullptr;
        return;
    }

    using U = std::remove_pointer_t<P>;
    if (std::holds_alternative<JsonArray>(jv.get_value())) {
        const auto& arr = jv.get_value_by_index<JsonArray, 6>();
        const std::size_t N = arr.size();
        U* heapArr = new U[N];
        for (std::size_t i = 0; i < N; i++) {
            from_json(heapArr[i], arr[i]);
        }
        out = heapArr;
        return;
    }

    U* single = new U{};
    from_json(*single, jv);
    out = single;
}

template<typename C>
requires std::is_class_v<C>
&&  requires(C &inst)
{
    {
        deserialize(inst, [](auto const &key, auto &field){})
    } -> std::same_as<void>;
}
void from_json(C& out, const JsonValue& jv) {
    if (jv.get_null()) {
        throw std::runtime_error("Null values should be wrapped in std::optional");
    }
    auto obj = jv.get_value_by_index<JsonObject, 5>();
    const JsonDescriptor jd{std::move(obj)};

    auto binder = [&](std::string const &key, auto& field) {
        const JsonValue& j = jd.get(key);
        from_json(field, j);
    };

    deserialize(out, binder);
}

template<Optional T>
void from_json(T& out, const JsonValue& jv) {
    using V = typename T::value_type;
    if constexpr (is_c_array<V>::value) throw std::runtime_error("C-style arrays should not be wrapped in std::optional");
    if constexpr (is_pointer<V>::value) throw std::runtime_error("Pointers should not be wrapped in std::optional");

    if (jv.get_null()) {
        out.reset();
    }
    else {
        throw std::runtime_error("Non null values should not be wrapped in std::optional");
    }
}
#endif //JSONOBJECT_H
