#include "parser/parse_tree.h"

bool ParseTreeNode::isCompound() {
    return token_.type() == TOKEN_ERR;
}

Token ParseTreeNode::token() {
    assert(!isCompound());
    return token_;
}

const std::vector<ParseTreePointer>& ParseTreeNode::children() {
    assert(isCompound());
    return children_;
}

void ParseTreeNode::emplace(ParseTreePointer node) {
    assert(isCompound());
    children_.emplace_back(node);
}