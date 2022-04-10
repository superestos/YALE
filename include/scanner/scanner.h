#pragma once

#include <vector>

#include "scanner/token.h"
#include "scanner/scan_stream.h"

class Scanner {
public:
    Scanner(std::istream& stream): tokenStream_{stream} {
        read();
    }

    std::vector<Token> tokens() const;
    void read();
    void clear();

private:
    ScanStream tokenStream_;
    std::vector<Token> tokens_;
};
