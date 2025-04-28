
#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <vector>
#include <string>
#include "TokenState.h"

#endif





class Tokenizer{
public:
    Tokenizer(std::string&& json) noexcept;
    std::vector<State::Token> Parse();
private:
    std::string json_value;
    char get_next_token_without_spaces();
    void check_token();
};