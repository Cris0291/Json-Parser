
#ifndef TOKENIZER_H
#define TOKENIZER_H

#endif

#include <string>
#include <iterator>


class Tokenizer{
public:
    Tokenizer(std::string&& json) noexcept;
    void Parse();
private:
    std::string json_value;
    char get_next_token_without_spaces();
    void check_token();
};