#include "expression/procedure.h"

#include "gtest/gtest.h"

#include "expression/expression.h"
#include "scanner/scanner.h"
#include "environment/environment.h"

class ProcedureTest : public ::testing::Test {
protected:
    EnvironmentManager manager_;
    EnvironmentPtr env_{manager_.global()};

    std::stringstream input_;
    Scanner scanner_{input_};
    Parser parser_;
};

TEST_F(ProcedureTest, Add) {
    AddProcedure add;
    std::vector<Value> args = {2, 4};

    EXPECT_EQ(add.call(env_, args).num(), 6);
}

TEST_F(ProcedureTest, SelfDefinedIdentity) {
    ExpressionPtr expr = std::shared_ptr<Expression>(new VariableExpression("x"));
    SelfDefinedProcedure identity(expr, {"x"});

    std::vector<Value> num_args = {42};
    EXPECT_EQ(identity.call(env_, num_args).num(), 42);

    std::vector<Value> quote_args = {Value("hi")};
    EXPECT_EQ(identity.call(env_, quote_args).quote(), "hi");
}
