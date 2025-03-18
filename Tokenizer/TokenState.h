
#ifndef TOKENSTATE_H
#define TOKENSTATE_H

#endif //TOKENSTATE_H

namespace State {
    enum class TokenState {
        NewToken,
        Symbol,
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

    };
}