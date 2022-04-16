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
    AddProcedure add = AddProcedure();
    std::vector<Value> args = {2, 4};

    EXPECT_EQ(add.call(env_, args).num(), 6);
}
