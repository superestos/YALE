#include <istream>
#include <iostream>

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
        return c == '(' || c == ')';
    }
};
