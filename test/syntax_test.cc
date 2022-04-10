#include "../src/syntax.h"

#include "gtest/gtest.h"

class SyntaxTest : public ::testing::Test {
protected:
    std::stringstream iss_;
    TokenStream s1_{iss_};
};

TEST_F(SyntaxTest, SimpleTokenStream) {
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
