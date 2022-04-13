#include "parser/parser.h"
#include "scanner/scanner.h"

#include "gtest/gtest.h"

class ParserTest : public ::testing::Test {
protected:
    std::stringstream input_;
    Scanner scanner_{input_};
};

TEST_F(ParserTest, SimpleParser) {
    Parser parser_;

    EXPECT_EQ(parser_.isValid(), true);
    EXPECT_EQ(parser_.tree().size(), 0);

    parser_.analyze(Token("1"));

    EXPECT_EQ(parser_.isValid(), true);
    EXPECT_EQ(parser_.tree().size(), 1);
}

TEST_F(ParserTest, SimpleParserWithParent) {
    Parser parser_;

    input_.clear();
    input_ << "(+ 1 2)";
    scanner_.read();
    parser_.analyze(scanner_.tokens());

    EXPECT_EQ(parser_.isValid(), true);
    EXPECT_EQ(parser_.tree().size(), 1);
    EXPECT_EQ(parser_.tree()[0]->children().size(), 3);
    EXPECT_EQ(parser_.tree()[0]->children()[0]->token().name(), "+");
    EXPECT_EQ(parser_.tree()[0]->children()[1]->token().name(), "1");
    EXPECT_EQ(parser_.tree()[0]->children()[2]->token().name(), "2");
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