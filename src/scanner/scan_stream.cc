#include "scanner/scan_stream.h"

const std::string ScanStream::next() {
    // read from the input stream
    if (read_input_) {
        stream_ >> buffer_;
    }

    // find the first char in buffer_ containing '(' or ')'
    size_t cut = buffer_.size();
    for (size_t i = 0; i < buffer_.size(); i++) {
        if (is_parentheses(buffer_[i])) {
            cut = std::max(1UL, i);
            break;
        }
    }

    // get the token and set buffer_ as the rest
    std::string token = buffer_.substr(0, cut);
    buffer_ = buffer_.substr(cut);
    read_input_ = buffer_.empty();
    
    return token;
}

bool ScanStream::is_parentheses(char c) {
    return c == '(' || c == ')';
}
