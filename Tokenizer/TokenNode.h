
#ifndef TOKENNODE_H
#define TOKENNODE_H
#include <memory>
#include <map>
#include <vector>

#endif //TOKENNODE_H



namespace Node {
    class TokenNode;
    using TokenObject = std::map<std::string, std::shared_ptr<TokenNode>>;
    using TokenList = std::vector<std::shared_ptr<TokenNode>>;
    using null = std:: nullptr_t;

    enum class Type {
        Object,
        Array,
        String_Literal,
        Numeric_Literal,
        Boolean,
        Null,
    };
    union Value {
        TokenObject *object;
        TokenList *list;
        std::string *s_literal;
        float n_literal;
        bool bValue;
        null nValue;
    };

    class TokenNode {
        Value value;
        Type type;
    public:
        TokenNode() = default;
        TokenNode(Type t, Value v);
        Type getType() const;
        Value getValue() const;
    };
}