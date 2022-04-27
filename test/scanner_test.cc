#include "scanner/scanner.h"

#include "gtest/gtest.h"

class ScannerTest : public ::testing::Test {
protected:
    std::stringstream input_;
    ScanStream ss_{input_};
    Scanner scanner_{input_};
};

TEST_F(ScannerTest, SimpleTokenStream) {
    input_.clear();
    input_ << "(+ 1 2)";
    
    EXPECT_EQ(ss_.next(), "(");
    EXPECT_EQ(ss_.next(), "+");
    EXPECT_EQ(ss_.next(), "1");
    EXPECT_EQ(ss_.next(), "2");
    EXPECT_EQ(ss_.next(), ")");
    EXPECT_EQ(ss_.next(), "");

    input_.clear();
    input_ << "hello world";
    EXPECT_EQ(ss_.next(), "hello");
    EXPECT_EQ(ss_.next(), "world");
    EXPECT_EQ(ss_.next(), "");    
}

TEST_F(ScannerTest, NestedTokenStream) {
    input_.clear();
    input_ << "(let ((val (+(* 1 2)3))) (* val 5))";
    
    EXPECT_EQ(ss_.next(), "(");
    EXPECT_EQ(ss_.next(), "let");
    EXPECT_EQ(ss_.next(), "(");
    EXPECT_EQ(ss_.next(), "(");
    EXPECT_EQ(ss_.next(), "val");
    EXPECT_EQ(ss_.next(), "(");
    EXPECT_EQ(ss_.next(), "+");
    EXPECT_EQ(ss_.next(), "(");
    EXPECT_EQ(ss_.next(), "*");
    EXPECT_EQ(ss_.next(), "1");
    EXPECT_EQ(ss_.next(), "2");
    EXPECT_EQ(ss_.next(), ")");
    EXPECT_EQ(ss_.next(), "3");
    EXPECT_EQ(ss_.next(), ")");
    EXPECT_EQ(ss_.next(), ")");
    EXPECT_EQ(ss_.next(), ")");
    EXPECT_EQ(ss_.next(), "(");
    EXPECT_EQ(ss_.next(), "*");
    EXPECT_EQ(ss_.next(), "val");
    EXPECT_EQ(ss_.next(), "5");
    EXPECT_EQ(ss_.next(), ")");
    EXPECT_EQ(ss_.next(), ")");
    EXPECT_EQ(ss_.next(), "");   
}

TEST(TokenTest, TokenType) {
    ASSERT_EQ(Token().type(), TOKEN_ERR);
    ASSERT_EQ(Token("").type(), TOKEN_ERR);
    ASSERT_EQ(Token("123").type(), TOKEN_NUM);
    ASSERT_EQ(Token("-123").type(), TOKEN_NUM);
    ASSERT_EQ(Token("1ab").type(), TOKEN_ERR);
    ASSERT_EQ(Token("-ab").type(), TOKEN_ERR);
    ASSERT_EQ(Token("L1").type(), TOKEN_ID);
    ASSERT_EQ(Token("'a").type(), TOKEN_QUOTE);
    ASSERT_EQ(Token("define").type(), TOKEN_DEFINE);
    ASSERT_EQ(Token("set!").type(), TOKEN_DEFINE);
    ASSERT_EQ(Token("let").type(), TOKEN_LET);
    ASSERT_EQ(Token("lambda").type(), TOKEN_LAMBDA);
    ASSERT_EQ(Token("cond").type(), TOKEN_COND);
    ASSERT_EQ(Token("(").type(), TOKEN_LPARENT);
    ASSERT_EQ(Token(")").type(), TOKEN_RPARENT);
    ASSERT_EQ(Token("+").type(), TOKEN_ID);
}

TEST_F(ScannerTest, SimpleScanner) {
    input_.clear();
    input_ << "(define val 1)";
    scanner_.read();

    EXPECT_EQ(scanner_.tokens().size(), 5);
    
    EXPECT_EQ(scanner_.tokens()[0].name(), "(");
    EXPECT_EQ(scanner_.tokens()[0].type(), TOKEN_LPARENT);
    EXPECT_EQ(scanner_.tokens()[1].name(), "define");
    EXPECT_EQ(scanner_.tokens()[1].type(), TOKEN_DEFINE);
    EXPECT_EQ(scanner_.tokens()[2].name(), "val");
    EXPECT_EQ(scanner_.tokens()[2].type(), TOKEN_ID);
    EXPECT_EQ(scanner_.tokens()[3].name(), "1");
    EXPECT_EQ(scanner_.tokens()[3].type(), TOKEN_NUM);
    EXPECT_EQ(scanner_.tokens()[4].name(), ")");
    EXPECT_EQ(scanner_.tokens()[4].type(), TOKEN_RPARENT);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
