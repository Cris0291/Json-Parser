#include "JsonValue.h"

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

template<typename T>
T JsonValue::get_value_by_index(FieldType type) const {
}