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

    TokenState stateNow = TokenState::NewToken;
    TokenState stateNext = TokenState::NewToken;

    TokenNode tokenNode{};

    int curlybraceBalance{0};
    std::string tokenValue {""};

    while (currChar != json_value.end()) {
        switch (stateNow) {
            case TokenState::NewToken: {
                tokenNode = TokenNode{Type::Unknown, Value{0}};

                if (lut::WhitespaceDigits.at((currChar[0]))) {
                    ++currChar;
                    stateNext = TokenState::NewToken;
                }
                else if (currChar[0] == '"') {
                    stateNext = TokenState::StringLiteral;
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
                ++currChar;
                stateNext = TokenState::NewToken;
            }
        }
        stateNow = stateNext;
    }
}

