#include "JsonDescriptor.h"

#include <stdexcept>

#include "DescriptorState.h"
#include "JsonValue.h"


void JsonDescriptor::createJsonMap(const std::vector<State::Token> &tokens) {
    auto currToken = tokens.begin();

    std::string currKey {};
    JsonValue currValue {};

    ParseState stateNow = ParseState::NewState;
    ParseState stateNext = ParseState::NewState;

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
                break;
            }
            case ParseState::KeyState: {
                currKey = currToken[0].value;
                ++currToken;
                stateNext = ParseState::NewState;
                break;
            }
            case ParseState::StringState: {
                currValue = JsonValue(currToken[0].value);
                ++currToken;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::NumberState: {
                if (currToken[0].jsonType == State::JsonState::ValueInt ) {
                    const auto result = parseJsonToken<int>(currToken[0].value);
                    currValue = JsonValue(result);
                }
                else {
                    const auto result =  parseJsonToken<double>(currToken[0].value);
                    currValue = JsonValue(result);
                }
                ++currToken;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::BoolState: {
                const bool result = parseJsonToken<bool>(currToken[0].value);
                currValue = JsonValue(result);
                ++currToken;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::CompleteState: {
                json_map.emplace(currKey, std::move(currValue));
                stateNext = ParseState::NewState;
                break;
            }
        }
        stateNow = stateNext;
    }
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



