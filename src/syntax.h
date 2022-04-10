#include <istream>
#include <iostream>
#include <vector>

typedef enum {
    TOKEN_ERR,
    TOKEN_NUM,
    TOKEN_QUOTE,
    TOKEN_ID,
    TOKEN_DEFINE,
    TOKEN_LET,
    TOKEN_LPARENT = '(',
    TOKEN_RPARENT = ')',
} TokenType;

class Token {
public:
    Token(std::string name): name_{name} {
        if (name.size() == 0) {
            type_ = TOKEN_ERR;
            return;
        }

        if (isNum(name[0])) {
            for (size_t i = 1; i < name.size(); i++) {
                if (!isNum(name[i])) {
                    type_ = TOKEN_ERR;
                    return;
                }
            }

            type_ = TOKEN_NUM;
            return;
        }

        if (name[0] == '\'') {
            type_ = TOKEN_QUOTE;
            return;
        }

        if (name == "define") {
            type_ = TOKEN_DEFINE;
            return;
        }

        if (name == "let") {
            type_ = TOKEN_LET;
            return;
        }

        if (name == "(") {
            type_ = TOKEN_LPARENT;
            return;
        }

        if (name == ")") {
            type_ = TOKEN_RPARENT;
            return;
        }

        type_ = TOKEN_ID;
    }

    TokenType type() {
        return type_;
    }

    std::string& name() {
        return name_;
    }

private:
    TokenType type_;
    std::string name_;

    bool isNum(char c) {
        return c >= '0' && c <= '9';
    }
};

class TokenStream {
public:
    TokenStream(std::istream& stream): stream_{stream} {}
    
    std::string& next() {
        if (read_) {
            stream_ >> buffer_;
        }

        size_t cut = buffer_.size();
        for (size_t i = 0; i < buffer_.size(); i++) {
            if (is_parentheses(buffer_[i])) {
                cut = std::max(1UL, i);
                break;
            }
        }

        token_ = buffer_.substr(0, cut);
        buffer_ = buffer_.substr(cut);
        read_ = buffer_.empty();
        
        return token_;
    }

private:
    std::istream& stream_;
    std::string buffer_;
    std::string token_;
    bool read_{true};

    bool is_parentheses(char c) {
        return c == TOKEN_LPARENT || c == TOKEN_RPARENT;
    }
};

class Syntax {
public:
    Syntax(std::istream& stream): tokenStream_{stream} {
        read();
    }

    std::vector<Token> tokens() const {
        return tokens_;
    }

    void read() {
        std::string name;
        while ((name = tokenStream_.next()) != "") {
            tokens_.emplace_back(name);
        }
    }

    void clear() {
        tokens_.clear();
    }

private:
    TokenStream tokenStream_;
    std::vector<Token> tokens_;
};
