#include "Tokenizer.h"
#include "TokenState.h"

#include <ranges>

#include "LookUpTable.h"
#include "TokenNode.h"
using namespace State;
using namespace  Node;

Tokenizer::Tokenizer(std::string &&json) noexcept : json_value{std::move(json)} {

}



void Tokenizer::Parse() {
    auto currChar = json_value.begin();
    std::vector<Token> vectorOutputTokens {};
    bool initMarker {true};
    bool keyStartMarker {true};

    TokenState stateNow = TokenState::NewToken;
    TokenState stateNext = TokenState::NewToken;
    JsonState jsonStateNow = JsonState::Unknown;

    std::string tokenValue {""};
    Token token{};

    int curlybraceBalance{0};


    while (currChar != json_value.end()) {
        switch (stateNow) {
            case TokenState::NewToken: {
                tokenValue.clear();
                token = {TokenState::Unknown, JsonState::Unknown, ""};

                if (lut::WhitespaceDigits.at((currChar[0]))) {
                    ++currChar;
                    stateNext = TokenState::NewToken;
                }
                else if (currChar[0] == '"') {
                    if (jsonStateNow == JsonState::Init) {
                        stateNext = TokenState::Key;
                    }
                    else {
                        stateNext = TokenState::StringLiteral;
                    }
                }
                else if (currChar[0] == '{') {
                    ++curlybraceBalance;
                    stateNext = TokenState::Open_Parenthesis;
                }
                else if (currChar[0] == '}') {
                    --curlybraceBalance;
                    stateNext = TokenState::Close_Parenthesis;
                }
                else if (currChar[0] == '[') {
                    stateNext = TokenState::Open_Array;
                }
                else if (currChar[0] == ']') {
                    stateNext = TokenState::Close_Array;
                }
                else if (currChar[0] == ':') {
                    stateNext = TokenState::Colon;
                }
                else if (currChar[0] == ',') {
                    stateNext = TokenState::Comma;
                }
                else if (lut::NumericDigits.at(currChar[0])) {
                    stateNext = TokenState::NumericLiteral;
                }
                else if (currChar[0] == 'b') {
                    stateNext = TokenState::Boolean;
                }
                else if (currChar[0] == 'f') {
                    stateNext = TokenState::Boolean;
                }
                break;
            }
            case TokenState::Key: {
                if (keyStartMarker) {
                    if (!lut::KeyValidStart.at(currChar[0]))throw std::invalid_argument("Json key should start with letters or an underscore");
                    keyStartMarker = false;
                }
                if (currChar[0] == '"') {
                    token = {TokenState::Key, JsonState::Key, tokenValue};
                    jsonStateNow = JsonState::Key;
                    stateNext = TokenState::CompleteToken;
                }
                tokenValue += currChar[0];
                ++currChar;
                break;
            }
            case TokenState::StringLiteral: {
                if (currChar[0] != '"') {
                    tokenValue += currChar[0];
                    ++currChar;
                }
                else {
                    ++currChar[0];
                    stateNext = TokenState::CompleteToken;
                }
            }
            case TokenState::Open_Array: {
                tokenValue += currChar[0];
                token = {TokenState::Open_Array, tokenValue};
                ++currChar;
                stateNext = TokenState::NewToken;
            }
            case TokenState::Open_Parenthesis: {
                tokenValue += currChar[0];
                stateNext = TokenState::CompleteToken;

                if (initMarker && std::distance(json_value.begin(), currChar) == 0) {
                    token = {TokenState::Open_Parenthesis, JsonState::Init, tokenValue};
                    jsonStateNow = JsonState::Init;
                    initMarker = false;
                }
                else {
                    token = {TokenState::Open_Parenthesis, JsonState::Value, tokenValue};
                    jsonStateNow = JsonState::Value;
                }
                break;
            }
            case TokenState::CompleteToken: {
                vectorOutputTokens.push_back(token);
                stateNext = TokenState::NewToken;
                break;
            }

        }
        stateNow = stateNext;
    }
}

