#include "../src/syntax.h"

#include "gtest/gtest.h"

class SyntaxTest : public ::testing::Test {
protected:
    std::stringstream iss_;
    Syntax s1_{iss_};
};

TEST_F(SyntaxTest, NewLineTest) {
    iss_ << "hello world";
    EXPECT_EQ(s1_.newToken(), "hello");
    EXPECT_EQ(s1_.newToken(), "world");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
