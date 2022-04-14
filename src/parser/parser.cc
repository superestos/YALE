#include "parser/parser.h"

void Parser::analyze(Token token) {
    if (token.type() == TOKEN_LPARENT) {
        auto node = std::make_shared<ParseTreeNode>();
        stack_.top()->emplace(node);
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
        stack_.top()->emplace(node);
    }
}
