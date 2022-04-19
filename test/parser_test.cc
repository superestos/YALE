#include "parser/parser.h"
#include "scanner/scanner.h"

#include "gtest/gtest.h"

class ParserTest : public ::testing::Test {
protected:
    std::stringstream input_;
    Scanner scanner_{input_};
    Parser parser_;
};

TEST_F(ParserTest, Simple) {
    EXPECT_EQ(parser_.valid(), true);

    parser_.analyze(Token("1"));

    EXPECT_EQ(parser_.valid(), true);
    EXPECT_EQ(parser_.has_next(), true);
}

TEST_F(ParserTest, MutlipleCommand) {
    input_.clear();
    input_ << "x 1";
    scanner_.read();
    parser_.analyze(scanner_.tokens());

    EXPECT_EQ(parser_.valid(), true);

    EXPECT_EQ(parser_.has_next(), true);
    EXPECT_EQ(parser_.next()->token().name(), "x");
    EXPECT_EQ(parser_.has_next(), true);
    EXPECT_EQ(parser_.next()->token().name(), "1");
    EXPECT_EQ(parser_.has_next(), false);
}


TEST_F(ParserTest, SingleParent) {
    input_.clear();
    input_ << "(+ 1 2)";
    scanner_.read();
    parser_.analyze(scanner_.tokens());

    EXPECT_EQ(parser_.valid(), true);
    EXPECT_EQ(parser_.has_next(), true);
    auto tree = parser_.next();
    EXPECT_EQ(parser_.has_next(), false);

    EXPECT_EQ(tree->children().size(), 3);
    EXPECT_EQ(tree->children()[0]->token().name(), "+");
    EXPECT_EQ(tree->children()[1]->token().name(), "1");
    EXPECT_EQ(tree->children()[2]->token().name(), "2");
}

TEST_F(ParserTest, NestedParent) {
    input_.clear();
    input_ << "(+ (f x) (g x))";
    scanner_.read();
    parser_.analyze(scanner_.tokens());

    EXPECT_EQ(parser_.valid(), true);
    EXPECT_EQ(parser_.has_next(), true);
    auto tree = parser_.next();
    EXPECT_EQ(parser_.has_next(), false);

    EXPECT_EQ(tree->children().size(), 3);
    EXPECT_EQ(tree->children()[0]->token().name(), "+");
    EXPECT_EQ(tree->children()[1]->isCompound(), true);
    EXPECT_EQ(tree->children()[2]->isCompound(), true);
    EXPECT_EQ(tree->children()[2]->children()[0]->token().name(), "g");
}

TEST_F(ParserTest, PartialInput) {
    input_.clear();
    input_ << "(+ 1";
    scanner_.read();
    parser_.analyze(scanner_.tokens());

    EXPECT_EQ(parser_.valid(), false);
    EXPECT_EQ(parser_.has_next(), false);
    
    input_.clear();
    input_ << "2 )";
    scanner_.clear();
    scanner_.read();
    parser_.analyze(scanner_.tokens());

    EXPECT_EQ(parser_.valid(), true);
    EXPECT_EQ(parser_.has_next(), true);
    auto tree = parser_.next();
    EXPECT_EQ(parser_.has_next(), false);

    EXPECT_EQ(tree->children().size(), 3);
    EXPECT_EQ(tree->children()[0]->token().name(), "+");
    EXPECT_EQ(tree->children()[1]->token().name(), "1");
    EXPECT_EQ(tree->children()[2]->token().name(), "2");
}

TEST_F(ParserTest, InvalidParentInput) {
    input_.clear();
    input_ << "(+ (f x)) (g x))";
    scanner_.read();
    parser_.analyze(scanner_.tokens());

    EXPECT_EQ(parser_.valid(), false);
}

TEST_F(ParserTest, InvalidTokenInput) {
    input_.clear();
    input_ << "1a";
    scanner_.read();
    parser_.analyze(scanner_.tokens());

    EXPECT_EQ(parser_.valid(), false);
}

TEST(ParseTreeTest, ParseType) {
    ParseTreePointer elementTree = std::make_shared<ParseTreeNode>(Token("1"));
    ParseTreePointer compoundTree = std::make_shared<ParseTreeNode>();
    
    compoundTree->emplace(std::make_shared<ParseTreeNode>(Token("(")));
    compoundTree->emplace(std::make_shared<ParseTreeNode>(Token("f")));
    compoundTree->emplace(std::make_shared<ParseTreeNode>(Token("x")));
    compoundTree->emplace(std::make_shared<ParseTreeNode>(Token(")")));

    ASSERT_EQ(elementTree->isCompound(), false);
    ASSERT_EQ(elementTree->token().type(), TOKEN_NUM);
    ASSERT_EQ(compoundTree->isCompound(), true);
    ASSERT_EQ(compoundTree->children()[1]->token().type(), TOKEN_ID);
}
