#include <istream>
#include <iostream>

class TokenStream {
public:
    TokenStream(std::istream& stream): stream_{stream} {}
    std::string& newToken() {
        if (read_) {
            stream_ >> buffer_;
        }
        read_ = true;

        for (size_t i = 0; i < buffer_.size(); i++) {
            if (buffer_[i] == '(' || buffer_[i] == ')') {
                size_t cut = std::max(1UL, i);
                read_ = false;

                token_ = buffer_.substr(0, cut);
                buffer_ = buffer_.substr(cut);
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

    const std::string left_parentheses_{"("};
    const std::string right_parentheses_{")"};
};
