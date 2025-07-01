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
                else if (currToken[0].type == State::TokenState::Null) {
                    stateNext = ParseState::Null;
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
            case ParseState::Null: {
                const auto result = parseJsonToken<std::monostate>(currToken[0].value);
                if (currState == 0) currValue = JsonValue(result);
                if (currState == 1 || currState == 2) {
                    recursive_state.top().currValue = JsonValue(result);
                }
                ++currToken;
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


JsonDescriptor::JsonDescriptor(std::unordered_map<std::string, JsonValue> map) : json_map(std::move(map)) {
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
    else if constexpr (std::is_same_v<T, std::monostate>) {
        return {};
    }
    else {
        throw std::invalid_argument("Unsupported type while parsing the json token");
    }
}

template<typename T>
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
        throw std::runtime_error("Null values should not be wrapped in std::optional");
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
    else if constexpr (std::is_same_v<T, std::string>) {
        out = jv.get_value_by_index<std::string ,4>();
    }
}

template<Containerable C>
void from_json(C& out, const JsonValue& jv) {
    if (jv.get_null()) {
        throw std::runtime_error("Null values should not be wrapped in std::optional");
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
        throw std::runtime_error("Null values should not be wrapped in std::optional");
    }
    //using inner_elem = typename A::value_type;
    constexpr std::size_t N = std::tuple_size_v<A>;

    if (jv.get_null()) {

        return;
    }

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

    const auto& arr = jv.get_value_by_index<JsonArray, 7>();
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
        const auto& arr = jv.get_value_by_index<JsonArray, 7>();
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
void from_json(C& out, const JsonValue& jv) {
    if (jv.get_null()) {
        throw std::runtime_error("Null values should not be wrapped in std::optional");
    }
    auto obj = jv.get_value_by_index<JsonObject, 6>();
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
