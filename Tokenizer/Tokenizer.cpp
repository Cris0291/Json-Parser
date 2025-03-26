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
    bool numericPointFound {false};
    bool numericWhiteSpaceFound {false};
    bool openCurlyBracesValue {false};

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
                    if (jsonStateNow == JsonState::Init || jsonStateNow == JsonState::Comma || openCurlyBracesValue) {
                        stateNext = TokenState::Key;
                        ++currChar;
                    }
                    else {
                        stateNext = TokenState::StringLiteral;
                        ++currChar;
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
                else if (currChar[0] == 't') {
                    stateNext = TokenState::TrueBoolean;
                }
                else if (currChar[0] == 'f') {
                    stateNext = TokenState::FalseBoolean;
                }
                else {
                    throw std::invalid_argument("Json was ill-formed");
                }
                break;
            }
            case TokenState::Key: {
                if (keyStartMarker) {
                    if (!lut::KeyValidStart.at(currChar[0]))throw std::invalid_argument("Json key could not be parser into a  valid c++ variable. C++ variables should start with letters or an underscore");
                    keyStartMarker = false;
                }

                if (currChar[0] == '"') {
                    token = {TokenState::Key, JsonState::Key, tokenValue};
                    jsonStateNow = JsonState::Key;
                    stateNext = TokenState::CompleteToken;
                    keyStartMarker = true;
                    ++currChar;
                    break;
                }

                if (!lut::KeyDigits.at(currChar[0])) {
                    throw std::invalid_argument("Either key contains invalid characters and could not be parser into a valid c++ variable or was lacking a final \"");
                }

                tokenValue += currChar[0];
                ++currChar;
                break;
            }
            case TokenState::StringLiteral: {
                if (jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                if (currChar[0] == '"') {
                    stateNext = TokenState::CompleteToken;
                    jsonStateNow = JsonState::Value;
                    token = {TokenState::StringLiteral, JsonState::Value, tokenValue};
                    ++currChar;
                }
                tokenValue += currChar[0];
                ++currChar;
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
                    openCurlyBracesValue = true;
                }
                break;
            }
            case TokenState::NumericLiteral: {
                if (jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or was not constructed correctly");
                if (lut::RealNumericDigits.at(currChar[0])) {
                    if (numericWhiteSpaceFound) {
                        throw std::invalid_argument("Bad numeric construction");
                    }
                    if (currChar[0] == '.') {
                        if (numericPointFound) {
                            throw std::invalid_argument("Bad numeric construction");
                        }
                        numericPointFound = true;
                    }
                    tokenValue += currChar[0];
                    ++currChar;
                    stateNext = TokenState::NumericLiteral;
                }
                else {
                    if (currChar[0] == ',') {
                        token = {TokenState::NumericLiteral, JsonState::Value, tokenValue};
                        jsonStateNow = JsonState::Value;
                        stateNext = TokenState::CompleteToken;
                        numericPointFound = false;
                        numericWhiteSpaceFound = false;
                        break;
                    }

                    if (lut::WhitespaceDigits.at((currChar[0]))) {
                        numericWhiteSpaceFound = true;
                        ++currChar;
                        stateNext = TokenState::NumericLiteral;
                    }

                   if (lut::StringDigits.at(currChar[0])){
                       throw std::invalid_argument("Invalid number");
                   }
                }
            }
            break;
            case TokenState::TrueBoolean: {
                if (jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or was not constructed correctly");
                if (currChar[0] == ',') {
                    if (tokenValue.size() != 4) throw std::invalid_argument("true boolean was not formed correctly");

                    token = {TokenState::TrueBoolean, JsonState::Value, tokenValue};
                    jsonStateNow = JsonState::Value;
                    stateNext = TokenState::CompleteToken;
                    break;
                }
                if (!lut::TrueBooleanDigits.at(currChar[0])) throw std::invalid_argument("true boolean was not formed correctly");

                tokenValue += currChar[0];
                ++currChar;
                break;
            }
            case TokenState::FalseBoolean: {
                if (jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or was not constructed correctly");
                if (currChar[0] == ',') {
                    if (tokenValue.size() != 4) throw std::invalid_argument("false boolean was not formed correctly");

                    token = {TokenState::FalseBoolean, JsonState::Value, tokenValue};
                    jsonStateNow = JsonState::Value;
                    stateNext = TokenState::CompleteToken;
                    break;
                }
                if (!lut::FalseBooleanDigits.at(currChar[0])) throw std::invalid_argument("false boolean was not formed correctly");

                tokenValue += currChar[0];
                ++currChar;
                break;
            }
            case TokenState::Colon: {
                if (jsonStateNow != JsonState::Key) throw std::invalid_argument("Json was ill-formed. Either a key was missing or was not constructed correctly");

                tokenValue += currChar[0];
                token = {TokenState::Colon, JsonState::Colon, tokenValue};
                ++currChar;
                jsonStateNow = JsonState::Colon;
                stateNext = TokenState::CompleteToken;
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
    if (stateNow == TokenState::StringLiteral || stateNow == TokenState::Key) {
        throw std::invalid_argument("Missing quotation mark");
    }
}

