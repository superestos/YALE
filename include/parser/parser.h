#include <stack>

#include "parser/parse_tree.h"

class Parser {
public:
    /*
    Parser() {
        tree_ = std::make_shared<ParseTreeNode>();
        stack_.push(tree_);
    }
    */

    void analyze(Token token);

    void analyze(const std::vector<Token>& tokens) {
        for (auto& token: tokens) {
            analyze(token);
        }
    }

    ParseTreePointer next() {
        ParseTreePointer ptr = trees_.front();
        trees_.pop_front();
        return ptr;
    }

    bool has_next() {
        return !trees_.empty();
    }

    bool isValid() {
        return stack_.empty() && !error_;
    }

private:
    std::deque<ParseTreePointer> trees_;
    std::stack<ParseTreePointer> stack_;
    bool error_{false};

    void insert(ParseTreePointer ptr);
};