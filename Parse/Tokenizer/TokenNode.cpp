#include "TokenNode.h"

using namespace Node;

TokenNode::TokenNode(const Type t, const Value v) : value{v}, type{t}{

}
Type TokenNode::getType() const {
    return type;
}
Value TokenNode::getValue() const {
    return value;
}
