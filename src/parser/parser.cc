#include "parser/parser.h"

void Parser::analyze(Token token) {
    if (token.type() == TOKEN_LPARENT) {
        auto node = std::make_shared<ParseTreeNode>();
        insert(node);
        stack_.push(node);

    } else if (token.type() == TOKEN_RPARENT) {
        if (isValid()) {
            error_ = true;
        } else {
            stack_.pop();
        }

    } else if (token.type() == TOKEN_ERR) {
        error_ = true;
        
    } else {
        auto node = std::make_shared<ParseTreeNode>(token);
        insert(node);
    }
}

void Parser::insert(ParseTreePointer ptr) {
    if (stack_.empty()) {
        trees_.emplace_back(ptr);
    } else {
        stack_.top()->emplace(ptr);
    }
}
