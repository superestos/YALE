#include <istream>
#include <iostream>

class TokenStream {
public:
    TokenStream(std::istream& stream): stream_{stream} {}
    std::string& next() {
        if (read_) {
            stream_ >> buffer_;
        }
        read_ = true;

        for (size_t i = 0; i < buffer_.size(); i++) {
            if (is_parentheses(buffer_[i])) {
                size_t cut = std::max(1UL, i);
                read_ = buffer_.size() == 1;

                if (!read_) {
                    token_ = buffer_.substr(0, cut);
                    buffer_ = buffer_.substr(cut);
                }
                
                break;
            }
        }

        if (read_) {
            token_ = std::move(buffer_);
        }
        
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
