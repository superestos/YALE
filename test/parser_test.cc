#include "parser/parser.h"
#include "scanner/scanner.h"

#include "gtest/gtest.h"

class ParserTest : public ::testing::Test {
protected:
    std::stringstream input_;
    Scanner scanner_{input_};
    Parser parser_;
};

TEST_F(ParserTest, SimpleParser) {
    EXPECT_EQ(parser_.isValid(), true);
    EXPECT_EQ(parser_.tree().size(), 0);

    parser_.analyze(Token("1"));

    EXPECT_EQ(parser_.isValid(), true);
    EXPECT_EQ(parser_.tree().size(), 1);
}

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
