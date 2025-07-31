
#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "JsonDescriptor.h"

#endif





class Tokenizer{
public:
    explicit Tokenizer(std::string&& json) noexcept;
    explicit Tokenizer(const std::filesystem::path &file_path);
    std::vector<State::Token> Parse();
    void CreateTokens() const;
private:
    std::string json_value;
    std::vector<State::Token> tokens;
};