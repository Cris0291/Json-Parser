
#ifndef TOKENIZER_H
#define TOKENIZER_H

#endif

#include <string>
#include <iterator>

template<typename IteratorType>
class Tokenizer{
public:
    Tokenizer(std::string&& json) noexcept;
    void parser();
private:
    std::string json_value;
    typename std::iterator_traits<IteratorType>::value_type current_position;
    int position;
    char get_next_token_without_spaces();
};