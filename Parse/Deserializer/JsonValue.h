//
// Created by cris0 on 5/2/2025.
//

#ifndef JSONVALUE_H
#define JSONVALUE_H
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>
#include "JsonDescriptor.h"

class JsonValue;

using JsonObject = std::unordered_map<std::string, JsonValue>;
using JsonArray = std::vector<JsonValue>;

class JsonValue {
public:
    using ValueType = std::variant<std::monostate,int,double,bool,std::string, JsonObject, JsonArray>;
    JsonValue() = default;
    JsonValue(const JsonValue&) = default;
    JsonValue(JsonValue&&) = default;
    JsonValue& operator= (const JsonValue&) = default;
    JsonValue& operator= (JsonValue&&) = default;
    template<typename T>
    JsonValue(T val);
    template<typename T, int index>
    T get_value_by_index() const;
    ValueType get_value() const;
    void set_null();
    bool get_null() const;
private:
    ValueType _value;
    bool _isNull = false;
};




struct JsonRecursiveToken {
    int state;
    std::string currKey {};
    JsonValue currValue {};
    std::variant<JsonObject, JsonArray> value;
    JsonRecursiveToken(const int s, std::variant<JsonObject, JsonArray>&& v) : state{s}, value{std::move(v)}{}
};

void updateVariant(JsonRecursiveToken& token);

#endif //JSONVALUE_H
