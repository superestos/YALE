#include <iostream>
#include <fstream>

#include "scanner/scanner.h"
#include "parser/parser.h"
#include "environment/environment.h"

struct Evaluator {
    EnvironmentManager manager_{ENV_BUILTIN};
    EnvironmentPtr env_{manager_.global()};

    Scanner scanner_;
    Parser parser_;

    Evaluator(std::istream &is): scanner_{is} {}

    void read() {
        scanner_.read();
        parser_.analyze(scanner_.tokens());
    }

    auto eval() {
        read();
        return Expression::create(parser_.next())->eval(env_);
    }
};

int main(int argc, char *argv[]) {
    std::string cursor = ">";

    if (argc == 1) {
        Evaluator evaluator(std::cin);
        auto value = evaluator.eval();
        std::cout << value << std::endl;

    } else if (argc == 2) {
        std::string filename(argv[1]);
        std::ifstream file;
        file.open(filename);
        if (!file.is_open()) {
            std::cerr << "file " << filename << " doesn't exist!" << std::endl;
        }

        Evaluator evaluator(file);
        auto value = evaluator.eval();
        std::cout << value << std::endl;

    } else {
        std::cout << "usage: " << std::string(argv[0]) << " [file]" << std::endl;
    }

    return 0;
}