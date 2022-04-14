#include "environment/environment.h"

#include "gtest/gtest.h"

class EnvironmentTest : public ::testing::Test {
protected:
    EnvironmentManager manager_;
    EnvironmentPtr global_{manager_.global()};
    EnvironmentPtr local_{manager_.create(global_)};
};

TEST_F(EnvironmentTest, SimpleEnvironment) {
    ASSERT_EQ(global_->existed("x"), false);

    global_->define("x", Value(42));
    ASSERT_EQ(global_->existed("x"), true);
    ASSERT_EQ(global_->get("x").num(), 42);

    global_->set("x", Value(40));
    ASSERT_EQ(global_->existed("x"), true);
    ASSERT_EQ(global_->get("x").num(), 40);
}

TEST_F(EnvironmentTest, EnclosingEnvironment) {
    ASSERT_EQ(local_->existed("x"), false);

    global_->define("x", Value(42));

    ASSERT_EQ(local_->existed("x"), true);
    ASSERT_EQ(local_->get("x").num(), 42);

    global_->set("x", Value(40));
    ASSERT_EQ(local_->existed("x"), true);
    ASSERT_EQ(local_->get("x").num(), 40);

    local_->define("x", Value(42));
    ASSERT_EQ(local_->existed("x"), true);
    ASSERT_EQ(local_->get("x").num(), 42);
}
