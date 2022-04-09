#include <istream>

class Syntax {
public:
    Syntax(std::istream& stream): stream_{stream} {}
    std::string& newToken() {
        stream_ >> token_;
        return token_;
    }

private:
    std::istream& stream_;
    std::string token_;
};
