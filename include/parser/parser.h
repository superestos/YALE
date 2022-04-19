#include <stack>

#include "parser/parse_tree.h"

class Parser {
public:
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
        return trees_.size() > 1 || (trees_.size() == 1 && valid());
    }

    bool valid() {
        return stack_.empty() && !error_;
    }

private:
    std::deque<ParseTreePointer> trees_;
    std::stack<ParseTreePointer> stack_;
    bool error_{false};

    void insert(ParseTreePointer ptr);
};