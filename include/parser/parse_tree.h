#pragma once

#include <vector>
#include <memory>
#include <cassert>

#include "scanner/token.h"

class ParseTreeNode;
typedef std::shared_ptr<ParseTreeNode> ParseTreePointer;

class ParseTreeNode {
public:
    ParseTreeNode(Token token): token_{token} {}

    ParseTreeNode(): token_{} {}

    bool isCompound();
    Token token(); 
    const std::vector<ParseTreePointer>& children();

    void emplace(ParseTreePointer node);

    static ParseTreePointer create() {
        return std::make_shared<ParseTreeNode>();
    }

    static ParseTreePointer create(Token token) {
        return std::make_shared<ParseTreeNode>(token);
    }

private:
    Token token_;
    std::vector<ParseTreePointer> children_;
};


