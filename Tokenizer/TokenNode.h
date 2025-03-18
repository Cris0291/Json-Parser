
#ifndef TOKENNODE_H
#define TOKENNODE_H
#include <memory>
#include <map>
#include <vector>

#endif //TOKENNODE_H



namespace Node {
    struct TokenNode;
    using TokenObject = std::map<std::string, std::shared_ptr<TokenNode>>;
    using TokenList = std::vector<std::shared_ptr<TokenNode>>;
    using null = std:: nullptr_t;

    enum class Type {
        Object,
        Array,
        String_Literal,
        Numeric_Literal,
        Boolean,
    };

    struct TokenNode {
        union Value {
            TokenObject *object;
            TokenList *list;
            std::string *s_literal;
            float n_literal;
            bool bValue;
            null nValue;
        } value;
        Type type;
    };
}