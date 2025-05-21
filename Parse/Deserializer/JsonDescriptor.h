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
};
template<typename T>
constexpr FieldType getType();

#endif //JSONOBJECT_H
