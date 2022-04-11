#include "parser/parse_tree.h"

#include "gtest/gtest.h"

TEST(ParseTreeTest, ParseType) {
    ParseTreePointer elementTree = std::make_shared<ParseTreeNode>(Token("1"));
    ParseTreePointer compoundTree = std::make_shared<ParseTreeNode>();
    
    compoundTree->children().emplace_back(std::make_shared<ParseTreeNode>(Token("(")));
    compoundTree->children().emplace_back(std::make_shared<ParseTreeNode>(Token("f")));
    compoundTree->children().emplace_back(std::make_shared<ParseTreeNode>(Token("x")));
    compoundTree->children().emplace_back(std::make_shared<ParseTreeNode>(Token(")")));

    ASSERT_EQ(elementTree->isCompound(), false);
    ASSERT_EQ(elementTree->token().type(), TOKEN_NUM);
    ASSERT_EQ(compoundTree->isCompound(), true);
    ASSERT_EQ(compoundTree->children()[1]->token().type(), TOKEN_ID);
}
