#include "JsonDescriptor.h"

#include <stack>
#include <stdexcept>

#include "DescriptorState.h"
#include "JsonValue.h"


void JsonDescriptor::createJsonMap(const std::vector<State::Token> &tokens) {
    auto currToken = tokens.begin();

    std::string currKey {};
    JsonValue currValue {};

    ParseState stateNow = ParseState::NewState;
    ParseState stateNext = ParseState::NewState;

    std::stack<JsonRecursiveToken> recursive_state {};
    int currState {0};

    bool isClosedObject {false};
    bool isClosedArray {false};

    while (currToken != tokens.end()) {
        switch (stateNow) {
            case ParseState::NewState: {
                if (currToken[0].type == State::TokenState::Key) {
                    stateNext = ParseState::KeyState;
                }
                else if (currToken[0].type == State::TokenState::StringLiteral) {
                    stateNext = ParseState::StringState;
                }
                else if (currToken[0].type == State::TokenState::NumericLiteral) {
                    stateNext = ParseState::NumberState;
                }
                else if (currToken[0].type == State::TokenState::TrueBoolean) {
                    stateNext = ParseState::BoolState;
                }
                else if (currToken[0].type == State::TokenState::Open_Parenthesis) {
                    stateNext = ParseState::OpenObjectState;
                }
                else if (currToken[0].type == State::TokenState::Close_Parenthesis) {
                    stateNext = ParseState::CloseObjectState;
                }
                else if (currToken[0].type == State::TokenState::Open_Array) {
                    stateNext = ParseState::OpenArrayState;
                }
                else if (currToken[0].type == State::TokenState::Close_Array) {
                    stateNext = ParseState::CloseArrayState;
                }
                else {
                    ++currToken;
                    stateNext = ParseState::NewState;
                }
                break;
            }
            case ParseState::KeyState: {
                if (currState == 0) currKey = currToken[0].value;
                if (currState == 1 || currState == 2) {
                    recursive_state.top().currKey = currToken[0].value;
                }
                ++currToken;
                stateNext = ParseState::NewState;
                break;
            }
            case ParseState::StringState: {
                if (currState == 0) currValue = JsonValue(currToken[0].value);
                if (currState == 1 || currState == 2) {
                    recursive_state.top().currValue = JsonValue(currToken[0].value);
                }
                ++currToken;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::NumberState: {
                if (currToken[0].jsonType == State::JsonState::ValueInt ) {
                    const auto result = parseJsonToken<int>(currToken[0].value);
                    if (currState == 0) currValue = JsonValue(result);
                    if (currState == 1 || currState == 2) {
                        recursive_state.top().currValue = JsonValue(result);
                    }
                }
                else {
                    const auto result =  parseJsonToken<double>(currToken[0].value);
                    if (currState == 0) currValue = JsonValue(result);
                    if (currState == 1 || currState == 2) {
                        recursive_state.top().currValue = JsonValue(result);
                    }
                }
                ++currToken;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::BoolState: {
                const bool result = parseJsonToken<bool>(currToken[0].value);
                if (currState == 0) currValue = JsonValue(result);
                if (currState == 1 || currState == 2) {
                    recursive_state.top().currValue = JsonValue(result);
                }
                ++currToken;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::OpenObjectState: {
                if (currToken[0].jsonType == State::JsonState::Init) {
                    ++currToken;
                    stateNext = ParseState::NewState;
                    break;
                }
                currState = 1;
                ++currToken;
                JsonObject object {};
                auto recursive_token {JsonRecursiveToken(currState, std::move(object))};
                recursive_state.push(recursive_token);
                stateNext = ParseState::NewState;
                break;
            }
            case ParseState::CloseObjectState: {
                if (currState == 0) {
                    ++currToken;
                    stateNext = ParseState::CompleteState;
                    break;
                }
                ++currToken;
                isClosedObject = true;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::OpenArrayState: {
                currState = 2;
                ++currToken;
                JsonArray arr {};
                auto recursive_token {JsonRecursiveToken(currState, std::move(arr))};
                recursive_state.push(recursive_token);
                stateNext = ParseState::NewState;
                break;

            }
            case ParseState::CloseArrayState: {
                ++currState;
                isClosedArray = true;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::CompleteState: {
                if (currState == 0) json_map.emplace(currKey, std::move(currValue));
                if (currState == 1 || currState == 2) {
                    if (isClosedObject || isClosedArray) {
                        auto old_token = recursive_state.top().value;

                        if (recursive_state.size() > 1) {
                            recursive_state.pop();
                            auto& new_token = recursive_state.top();
                            new_token.currValue = isClosedObject ? std::get<JsonObject>(old_token) : std::get<JsonArray>(old_token);
                            currState = new_token.state;
                            updateVariant(new_token);
                        }
                        else {
                            recursive_state.pop();
                            if (!recursive_state.empty()) throw std::invalid_argument("Something unexpected happened");
                            currState = 0;
                            currValue = isClosedObject ? std::get<JsonObject>(old_token) : std::get<JsonArray>(old_token);
                            json_map.emplace(currKey, std::move(currValue));
                        }
                    }
                    else {
                        updateVariant(recursive_state.top());
                    }
                }
                stateNext = ParseState::NewState;
                break;
            }
        }
        stateNow = stateNext;
    }
}


JsonValue JsonDescriptor::get(const std::string &key) const {
    const auto iterator_value = json_map.find(key);
    if (iterator_value == json_map.end()) throw std::invalid_argument("Requested key was not found. " + key + " "+ "is not part of the json string");
    return iterator_value->second;
}

template<typename T>
T JsonDescriptor::parseJsonToken(const std::string &value) {
    if constexpr (std::is_same_v<T, int>) {
        return std::stoi(value);
    }
    else if constexpr (std::is_same_v<T, double>) {
        return std::stod(value);
    }
    else if constexpr (std::is_same_v<T, bool>) {
        return  value == "true";
    }
    else {
        throw std::invalid_argument("Unsupported type while parsing the json token");
    }
}

template<typename T>
T from_json(const JsonDescriptor &obj) {
    T instance {};

    auto binder = [&obj](const std::string& key, auto& field) {
        using raw_type = decltype(field);
        using field_type = std::remove_cvref_t<raw_type>;

        const JsonValue& jv = obj.get(key);

        field = from_json<field_type>(jv);
    };

    deserialize(instance, binder);

    return instance;
}

template<typename T>
requires (!Containerable<T>)
T from_json(const JsonValue& jv) {
    if constexpr (std::is_same_v<T, int>) {
        return jv.get_value_by_index<int, 1>();
    }
    else if constexpr (std::is_same_v<T, double>) {
        return jv.get_value_by_index<double, 2>();
    }
    else if constexpr (std::is_same_v<T, bool>) {
        return jv.get_value_by_index<bool, 3>();
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        return jv.get_value_by_index<std::string ,4>();
    }
}

template<Containerable C>
C from_Json(const JsonValue& jv) {
    C result;
    using inner_elem = typename C::value_type;
    const auto& arr = jv.get_value_by_index<JsonArray, 7>();

    if constexpr (requires(C& c, std::size_t n) {c.reseve(n);}) {
        result.reseve(arr.size());
    }

    for (const auto& a : arr) {
        result.insert(result.end(), from_json<inner_elem>(a));
    }
    return result;
}

template<typename A>
requires specialization_of_array<A>
A from_Json(const JsonValue& jv) {
    using inner_elem = typename A::value_type;
    constexpr std::size_t N = std::tuple_size_v<A>;

    const auto& arr = jv.get_value_by_index<JsonArray, 7>();
    if (arr.size() != N) {
        throw std::runtime_error(
        "JSON array size mismatch for std::array<…> expected "
        + std::to_string(N)
        + ", but got"
        + std::to_string(arr.size())
            );
    }

    A result;
    for (std::size_t i = 0; i < N; i++) {
        result[i] = from_json<inner_elem>(arr[i]);
    }
    return result;
}

template <typename T>
FieldType getType() {
    if constexpr (std::is_same_v<T, int>) return FieldType::Int;
    else if constexpr (std::is_same_v<T, std::string>) return FieldType::String;
    else if constexpr (std::is_same_v<T, bool>) return  FieldType::Boolean;
    else if constexpr (std::is_same_v<T, double>) return FieldType::Double;
    else if constexpr (is_specialization_of<T, std::vector>::value) return FieldType::Vector;
    else if constexpr (is_array_specialization<T>::value) return FieldType::Array;
    else if constexpr (is_c_array<T>::value) return  FieldType::CArray;
    else if constexpr (is_pointer<T>::value) return  FieldType::Pointer;
    else if constexpr (is_double_pointer<T>::value) return  FieldType::DoublePointer;
    else if constexpr  (std::is_class_v<T>) return FieldType::Object;
}
