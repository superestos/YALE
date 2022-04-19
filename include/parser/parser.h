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

    std::vector<ParseTreePointer> tree() {
        return trees_;
    }

    bool isValid() {
        return stack_.empty() && !error_;
    }

private:
    std::vector<ParseTreePointer> trees_;
    std::stack<ParseTreePointer> stack_;
    bool error_{false};

    void insert(ParseTreePointer ptr);
};