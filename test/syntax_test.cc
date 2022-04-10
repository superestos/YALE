#include "../src/syntax.h"

#include "gtest/gtest.h"

class SyntaxTest : public ::testing::Test {
protected:
    std::stringstream iss_;
    TokenStream s1_{iss_};
};

TEST_F(SyntaxTest, TokenStream) {
    iss_ << "(+ 1 2)";
    
    EXPECT_EQ(s1_.newToken(), "(");
    EXPECT_EQ(s1_.newToken(), "+");
    EXPECT_EQ(s1_.newToken(), "1");
    EXPECT_EQ(s1_.newToken(), "2");
    EXPECT_EQ(s1_.newToken(), ")");
    EXPECT_EQ(s1_.newToken(), "");

    iss_.clear();
    iss_ << "hello world";
    EXPECT_EQ(s1_.newToken(), "hello");
    EXPECT_EQ(s1_.newToken(), "world");
    EXPECT_EQ(s1_.newToken(), "");    
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
