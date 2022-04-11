#include "parser/parse_tree.h"

bool ParseTreeNode::isCompound() {
    return token_.type() == TOKEN_ERR;
}

Token ParseTreeNode::token() {
    assert(!isCompound());
    return token_;
}

std::vector<ParseTreePointer>& ParseTreeNode::children() {
    assert(isCompound());
    return children_;
}
