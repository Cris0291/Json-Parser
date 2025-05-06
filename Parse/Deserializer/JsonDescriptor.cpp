#include "JsonDescriptor.h"
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
            case ParseState::BoolState: {
                currValue = JsonValue(currToken[0].value);
                ++currToken;
                stateNext = ParseState::CompleteState;
                break;
            }
            case ParseState::CompleteState: {
                json_map[currKey] = currValue;
                stateNext = ParseState::NewState;
                break;
            }
        }
        stateNow = stateNext;
    }
}

template<typename T>
T JsonDescriptor::parseJsonToken(std::string value) {
    T result = static_cast<T>(value);
    return  result;
}


