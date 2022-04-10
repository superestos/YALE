#include "scanner/token.h"

class TokenStream {
public:
    TokenStream(std::istream& stream): stream_{stream} {}
    std::string& next();

private:
    std::istream& stream_;
    std::string buffer_;
    std::string token_;
    bool read_{true};

    bool is_parentheses(char c) {
        return c == TOKEN_LPARENT || c == TOKEN_RPARENT;
    }
};
