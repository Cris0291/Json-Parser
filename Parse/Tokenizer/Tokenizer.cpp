#include "Tokenizer.h"

#include <iostream>
#include <stack>
#include <fstream>
#include <filesystem>

#include "LookUpTable.h"
#include "TokenNode.h"

using namespace State;
using namespace  Node;

Tokenizer::Tokenizer(std::string &&json) noexcept : json_value{std::move(json)} {

}

Tokenizer::Tokenizer(const std::filesystem::path &file_path) {
    std::ifstream in{file_path, std::ios::binary};
    if (!in) {
        throw std::runtime_error("Cannot open JSON file: " + file_path.string());
    }

    json_value.assign(
        std::istreambuf_iterator<char>(in),
        std::istreambuf_iterator<char>()
        );
}


std::vector<Token> Tokenizer::Parse() {
    auto currChar = json_value.begin();
    std::vector<Token> vectorOutputTokens {};

    bool initMarker {true};
    bool keyStartMarker {true};
    bool numericPointFound {false};
    bool numericWhiteSpaceFound {false};
    bool isFirstNumeric {true};
    bool isExponent {false};

    TokenState stateNow = TokenState::NewToken;
    TokenState stateNext = TokenState::NewToken;
    JsonState jsonStateNow = JsonState::Unknown;

    std::string tokenValue {};
    Token token{};

    int curly_brace_balance{};
    int square_brace_balance {};

    std::stack<int> recursive_state{};
    recursive_state.push(0);

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
                    if (recursive_state.top() == 0) {
                        if (jsonStateNow == JsonState::Init || jsonStateNow == JsonState::Comma || jsonStateNow == JsonState::ValueOpenParenthesis) {
                            stateNext = TokenState::Key;
                            ++currChar;
                        }
                        else {
                            stateNext = TokenState::StringLiteral;
                            ++currChar;
                        }
                    }
                    else {
                        stateNext = TokenState::StringLiteral;
                        ++currChar;
                    }
                }
                else if (currChar[0] == '{') {
                    ++curly_brace_balance;
                    stateNext = TokenState::Open_Parenthesis;
                }
                else if (currChar[0] == '}') {
                    --curly_brace_balance;
                    stateNext = TokenState::Close_Parenthesis;
                }
                else if (currChar[0] == '[') {
                    ++square_brace_balance;
                    stateNext = TokenState::Open_Array;
                }
                else if (currChar[0] == ']') {
                    --square_brace_balance;
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
                else if (currChar[0] == 'n') {
                    stateNext = TokenState::Null;
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
                if (recursive_state.top() == 0 && jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                if(recursive_state.top() == 1) {
                    if (jsonStateNow != JsonState::Comma) {
                         if (jsonStateNow != JsonState::ValueOpenArray) {
                             throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                         }
                    }
                }

                if (currChar[0] == '"') {
                    stateNext = TokenState::CompleteToken;
                    jsonStateNow = JsonState::Value;
                    token = {TokenState::StringLiteral, JsonState::Value, tokenValue};
                    ++currChar;
                    break;
                }
                tokenValue += currChar[0];
                ++currChar;
                break;
            }
            case TokenState::Open_Array: {
                if (recursive_state.top() == 0 && jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                if(recursive_state.top() == 1) {
                    if (jsonStateNow != JsonState::Comma) {
                        if (jsonStateNow != JsonState::ValueOpenArray) {
                            throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                        }
                    }
                }

                tokenValue += currChar[0];
                ++currChar;
                jsonStateNow = JsonState::ValueOpenArray;
                stateNext = TokenState::CompleteToken;
                token = {TokenState::Open_Array, JsonState::ValueOpenArray, tokenValue};
                recursive_state.push(1);
                break;
            }
            case TokenState::Close_Array: {
                if (jsonStateNow == JsonState::Comma) throw std::invalid_argument("Trailing commas are not allowed");
                if (!(jsonStateNow == JsonState::Value || jsonStateNow == JsonState::ValueOpenArray)) throw std::invalid_argument("Json was ill-formed");

                tokenValue += currChar[0];
                ++currChar;
                stateNext = TokenState::CompleteToken;
                jsonStateNow = JsonState::Value;
                token = {TokenState::Close_Array, JsonState::Value, tokenValue};
                recursive_state.pop();
                break;
            }
            case TokenState::Open_Parenthesis: {
                tokenValue += currChar[0];
                ++currChar;
                stateNext = TokenState::CompleteToken;


                if (initMarker) {
                    token = {TokenState::Open_Parenthesis, JsonState::Init, tokenValue};
                    jsonStateNow = JsonState::Init;
                    initMarker = false;
                }
                else {
                    if (recursive_state.top() == 0 && jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                    if(recursive_state.top() == 1) {
                        if (jsonStateNow != JsonState::Comma) {
                            if (jsonStateNow != JsonState::ValueOpenArray) {
                                throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                            }
                        }
                    }

                    token = {TokenState::Open_Parenthesis, JsonState::ValueOpenParenthesis, tokenValue};
                    jsonStateNow = JsonState::ValueOpenParenthesis;
                    recursive_state.push(0);
                }
                break;
            }
            case TokenState::Close_Parenthesis: {
                if (jsonStateNow == JsonState::Comma) throw std::invalid_argument("Trailing commas are not allowed");
                if (!(jsonStateNow == JsonState::Value || jsonStateNow == JsonState::ValueOpenParenthesis)) throw std::invalid_argument("Json was ill-formed");
                recursive_state.pop();

                tokenValue += currChar[0];
                ++currChar;
                stateNext = TokenState::CompleteToken;
                jsonStateNow = JsonState::Value;
                token = {TokenState::Close_Parenthesis, JsonState::Value, tokenValue};
                break;
            }
            case TokenState::NumericLiteral: {
                if (recursive_state.top() == 0 && jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                if(recursive_state.top() == 1) {
                    if (jsonStateNow != JsonState::Comma) {
                         if (jsonStateNow != JsonState::ValueOpenArray) {
                             throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                         }
                    }
                }

                if (isFirstNumeric) {
                    if (currChar[0] == '-') {
                        tokenValue += currChar[0];
                        ++currChar;
                        isFirstNumeric = false;
                        break;
                    }
                    isFirstNumeric = false;
                    break;
                }
                if (currChar[0] == 'e' || currChar[0] == 'E') {
                    if (isExponent) {
                        throw std::invalid_argument("Bad numeric construction. Double exponent was found");
                    }

                    tokenValue += currChar[0];
                    ++currChar;
                    if (!(currChar[0] == '-' || currChar[0] == '+')) {
                        throw std::invalid_argument("Bad numeric construction. Exponent should be followed by + or -");
                    }
                    tokenValue += currChar[0];
                    ++currChar;
                    isExponent = true;
                    stateNext = TokenState::NumericLiteral;
                    break;
                }

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
                    break;
                }
                else {
                    if (currChar[0] == ',' || currChar[0] == '}' || currChar[0] == ']') {
                        token = {TokenState::NumericLiteral, numericPointFound ? JsonState::ValueDouble : JsonState::ValueInt, tokenValue};
                        jsonStateNow = JsonState::Value;
                        stateNext = TokenState::CompleteToken;
                        numericPointFound = false;
                        numericWhiteSpaceFound = false;
                        isFirstNumeric = true;
                        isExponent = false;
                        break;
                    }

                    if (lut::WhitespaceDigits.at((currChar[0]))) {
                        numericWhiteSpaceFound = true;
                        ++currChar;
                        stateNext = TokenState::NumericLiteral;
                        break;
                    }

                    throw std::invalid_argument("Invalid number");

                }
            }

            case TokenState::TrueBoolean: {
                if (recursive_state.top() == 0 && jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                if(recursive_state.top() == 1) {
                    if (jsonStateNow != JsonState::Comma) {
                        if (jsonStateNow != JsonState::ValueOpenArray) {
                            throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                        }
                    }
                }

                if (tokenValue.size() == 4 && lut::WhitespaceDigits.at((currChar[0]))) {
                    ++currChar;
                    break;
                }

                if (currChar[0] == ',' || currChar[0] == '}' || currChar[0] == ']') {
                    if (tokenValue.size() != 4 && tokenValue != "true") throw std::invalid_argument("true boolean was not formed correctly");

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
                if (recursive_state.top() == 0 && jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                if(recursive_state.top() == 1) {
                    if (jsonStateNow != JsonState::Comma) {
                        if (jsonStateNow != JsonState::ValueOpenArray) {
                            throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                        }
                    }
                }

                if (tokenValue.size() == 5 && lut::WhitespaceDigits.at((currChar[0]))) {
                    ++currChar;
                    break;
                }

                if (currChar[0] == ',' || currChar[0] == '}' || currChar[0] == ']') {
                    if (tokenValue.size() != 4 && tokenValue != "false") throw std::invalid_argument("false boolean was not formed correctly");

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
            case TokenState::Null: {
                if (recursive_state.top() == 0 && jsonStateNow != JsonState::Colon) throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                if(recursive_state.top() == 1) {
                    if (jsonStateNow != JsonState::Comma) {
                        if (jsonStateNow != JsonState::ValueOpenArray) {
                            throw std::invalid_argument("Json was ill-formed. Either a key was missing or something was not constructed correctly");
                        }
                    }
                }

                if (tokenValue.size() == 4 && lut::WhitespaceDigits.at((currChar[0]))) {
                    ++currChar;
                    break;
                }

                if (currChar[0] == ',' || currChar[0] == '}' || currChar[0] == ']') {
                    if (tokenValue.size() != 4 && tokenValue != "null") throw std::invalid_argument("null was not formed correctly");

                    token = {TokenState::Null, JsonState::Value, tokenValue};
                    jsonStateNow = JsonState::Value;
                    stateNext = TokenState::CompleteToken;
                    break;
                }
                if (!lut::NullDigits.at(currChar[0])) throw std::invalid_argument("null was not formed correctly");

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
            case TokenState::Comma: {
                if (jsonStateNow != JsonState::Value) throw std::invalid_argument("Json was ill-formed");
                tokenValue = currChar[0];
                ++currChar;
                stateNext = TokenState::CompleteToken;
                jsonStateNow = JsonState::Comma;
                token = {TokenState::Comma, JsonState::Comma, tokenValue};
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

    /*if (token.type != TokenState::Close_Parenthesis) throw std::invalid_argument("Json was ill-formed. Missing quotation a close }");
    vectorOutputTokens.push_back(token);*/

    if (stateNow == TokenState::StringLiteral || stateNow == TokenState::Key) {
        throw std::invalid_argument("Missing quotation mark");
    }

    return vectorOutputTokens;
}

