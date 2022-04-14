#include <istream>

class ScanStream {
public:
    ScanStream(std::istream& stream): stream_{stream} {}
    const std::string& next();

private:
    std::istream& stream_;
    std::string buffer_;
    std::string token_;
    bool read_{true};

    bool is_parentheses(char c);
};
