#include "JsonValue.h"

#include <stdexcept>

void updateVariant(JsonRecursiveToken &token) {
    std::visit([&](auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, JsonObject>) {
            val.emplace(std::move(token.currKey),std::move(token.currValue));
        }
        else if constexpr (std::is_same_v<T, JsonArray>) {
            val.push_back(std::move(token.currValue));
        }
    }, token.value);
}

template<typename T, int index>
T JsonValue::get_value_by_index() const {
    try {
        auto value = std::get<index>(this->_value);
        return value;
    }
    catch (const std::bad_variant_access&) {
        throw std::invalid_argument("Type mismatch. Current key type does not match with json's key value");
    }
}

JsonValue::ValueType JsonValue::get_value() const {
    return _value;
}

void JsonValue::set_null() {
    _isNull = true;
}

bool JsonValue::get_null() const {
    return _isNull;
}
