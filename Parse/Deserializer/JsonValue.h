//
// Created by cris0 on 5/2/2025.
//

#ifndef JSONVALUE_H
#define JSONVALUE_H
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


class JsonValue {
public:
    using JsonObject = std::unordered_map<std::string, JsonValue>;
    using JsonArray = std::vector<JsonValue>;
    using ValueType = std::variant<std::nullptr_t,int,double,bool,std::string, JsonObject, JsonArray>;
    JsonValue() = default;
    JsonValue(const JsonValue&) = default;
    JsonValue(JsonValue&&) = default;
    JsonValue& operator= (const JsonValue&) = default;
    JsonValue& operator= (JsonValue&&) = default;
    template<typename T>
    JsonValue(T val);
    template<typename T>
    T& get() const;
private:
    ValueType _value;
};



#endif //JSONVALUE_H
