
#ifndef TOKENSTATE_H
#define TOKENSTATE_H

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
        Boolean,
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