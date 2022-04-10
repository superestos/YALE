#include "../src/syntax.h"

#include "gtest/gtest.h"

class SyntaxTest : public ::testing::Test {
protected:
    std::stringstream iss_;
    TokenStream s1_{iss_};
};

TEST_F(SyntaxTest, SimpleTokenStream) {
    iss_.clear();
    iss_ << "(+ 1 2)";
    
    EXPECT_EQ(s1_.next(), "(");
    EXPECT_EQ(s1_.next(), "+");
    EXPECT_EQ(s1_.next(), "1");
    EXPECT_EQ(s1_.next(), "2");
    EXPECT_EQ(s1_.next(), ")");
    EXPECT_EQ(s1_.next(), "");

    iss_.clear();
    iss_ << "hello world";
    EXPECT_EQ(s1_.next(), "hello");
    EXPECT_EQ(s1_.next(), "world");
    EXPECT_EQ(s1_.next(), "");    
}

TEST_F(SyntaxTest, NestedTokenStream) {
    iss_.clear();
    iss_ << "(let ((val (+(* 1 2)3))) (* val 5))";
    
    EXPECT_EQ(s1_.next(), "(");
    EXPECT_EQ(s1_.next(), "let");
    EXPECT_EQ(s1_.next(), "(");
    EXPECT_EQ(s1_.next(), "(");
    EXPECT_EQ(s1_.next(), "val");
    EXPECT_EQ(s1_.next(), "(");
    EXPECT_EQ(s1_.next(), "+");
    EXPECT_EQ(s1_.next(), "(");
    EXPECT_EQ(s1_.next(), "*");
    EXPECT_EQ(s1_.next(), "1");
    EXPECT_EQ(s1_.next(), "2");
    EXPECT_EQ(s1_.next(), ")");
    EXPECT_EQ(s1_.next(), "3");
    EXPECT_EQ(s1_.next(), ")");
    EXPECT_EQ(s1_.next(), ")");
    EXPECT_EQ(s1_.next(), ")");
    EXPECT_EQ(s1_.next(), "(");
    EXPECT_EQ(s1_.next(), "*");
    EXPECT_EQ(s1_.next(), "val");
    EXPECT_EQ(s1_.next(), "5");
    EXPECT_EQ(s1_.next(), ")");
    EXPECT_EQ(s1_.next(), ")");
    EXPECT_EQ(s1_.next(), "");   
}

TEST(TokenTest, TokenType) {
    ASSERT_EQ(Token("").type(), TOKEN_ERR);
    ASSERT_EQ(Token("123").type(), TOKEN_NUM);
    ASSERT_EQ(Token("1ab").type(), TOKEN_ERR);
    ASSERT_EQ(Token("L1").type(), TOKEN_ID);
    ASSERT_EQ(Token("'a").type(), TOKEN_QUOTE);
    ASSERT_EQ(Token("define").type(), TOKEN_DEFINE);
    ASSERT_EQ(Token("let").type(), TOKEN_LET);
    ASSERT_EQ(Token("(").type(), TOKEN_LPARENT);
    ASSERT_EQ(Token(")").type(), TOKEN_RPARENT);
    ASSERT_EQ(Token("+").type(), TOKEN_ID);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
