#include "scanner/scan_stream.h"

std::string& TokenStream::next() {
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