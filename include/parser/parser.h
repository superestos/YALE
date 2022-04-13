#include <stack>

#include "parser/parse_tree.h"

class Parser {
public:
    Parser() {
        tree_ = std::make_shared<ParseTreeNode>();
        stack_.push(tree_);
    }

    void analyze(Token token);

    void analyze(std::vector<Token> tokens) {
        for (auto token: tokens) {
            analyze(token);
        }
    }

    std::vector<ParseTreePointer> tree() {
        return tree_->children();
    }

    bool isValid() {
        return stack_.size() == 1;
    }

private:
    ParseTreePointer tree_;
    std::stack<ParseTreePointer> stack_;
};