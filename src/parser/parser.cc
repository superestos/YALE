#include "parser/parser.h"

void Parser::analyze(Token token) {
    if (token.type() == TOKEN_LPARENT) {
        auto node = std::make_shared<ParseTreeNode>();
        stack_.top()->emplace(node);
        stack_.push(node);
    } else if (token.type() == TOKEN_RPARENT) {
        stack_.pop();
    } else {
        auto node = std::make_shared<ParseTreeNode>(token);
        stack_.top()->emplace(node);
    }
}
