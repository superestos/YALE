#include <istream>
/* ScanStream are used for split the tokens for input stream.
 * Tokens should be separated by space or parentheses. Space is not
 * considered as a token while a parenthese is a token.
 *
 */
class ScanStream {
public:
    ScanStream(std::istream& stream): stream_{stream} {}

    // iterator of ScanStream
    const std::string next();

private:
    std::istream& stream_; // input stream

    // buffer to store result of input stream,
    // which are not separated by parentheses
    std::string buffer_;

    // we should read input stream_ in the next iteration instead of buffer_
    bool read_input_{true};

    bool is_parentheses(char c);
};
