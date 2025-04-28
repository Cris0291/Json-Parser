
#ifndef TOKENSTATE_H
#define TOKENSTATE_H
#include <unordered_set>
#include <string>

#endif //TOKENSTATE_H

namespace State {
    enum class TokenState {
        NewToken,
        Key,
        StringLiteral,
        NumericLiteral,
        Open_Parenthesis,
        Close_Parenthesis,
        Open_Array,
        Close_Array,
        TrueBoolean,
        FalseBoolean,
        Null,
        Comma,
        Colon,
        CompleteToken,
        Unknown
    };
    enum class JsonState {
        Init,
        Key,
        Colon,
        Value,
        ValueOpenParenthesis,
        ValueOpenArray,
        Comma,
        Close,
        Unknown
    };
    struct Token {
        TokenState type;
        JsonState jsonType;
        std::string value;
    };
}