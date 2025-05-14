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
                break;
            }
            case ParseState::KeyState: {
                if (currState == 0) currKey = currToken[0].value;
                if (currState == 1) {
                    recursive_state.top().currKey = currToken[0].value;
                }
                ++currToken;
                stateNext = ParseState::NewState;
                break;
            }
            case ParseState::StringState: {
                if (currState == 0) currValue = JsonValue(currToken[0].value);
                if (currState == 1) {
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
                    if (currState == 1) {
                        recursive_state.top().currValue = JsonValue(result);
                    }
                }
                else {
                    const auto result =  parseJsonToken<double>(currToken[0].value);
                    if (currState == 0) currValue = JsonValue(result);
                    if (currState == 1) {
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
                if (currState == 1) {
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
                auto recursive_token {JsonRecursiveToken(currState, object)};
                recursive_state.push(recursive_token);
                stateNext = ParseState::NewState;
                break;
            }
            case ParseState::CloseObjectState: {
                ++currToken;
                isClosedObject = true;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::CompleteState: {
                if (currState == 0) json_map.emplace(currKey, std::move(currValue));
                if (currState == 1 || currState == 2) {
                    if (isClosedObject) {
                        auto old_token = recursive_state.top().value;

                        if (recursive_state.size() > 1) {
                            recursive_state.pop();
                            auto& new_token = recursive_state.top();
                            new_token.currValue = std::get<JsonObject>(old_token);
                            currState = new_token.state;
                        }
                        else {

                        }
                    }
                    else if (isClosedArray) {

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



