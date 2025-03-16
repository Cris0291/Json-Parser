#include "Tokenizer.h"

#include <ranges>

template<typename IteratorType>
Tokenizer<IteratorType>::Tokenizer(std::string &&json) noexcept : json_value{std::move(json)}, current_position{std::ranges::cbegin(json)}, position{0} {
}



template<typename IteratorType>
void Tokenizer<IteratorType>::parse() {
    if (json_value[0] != '{') throw std::invalid_argument{"Json value should start with { and end with }"};



}

template<typename IteratorType>
char Tokenizer<IteratorType>::get_next_token_without_spaces() {
    char result {};
    while (true) {
        if (*current_position == " " || *current_position == "\n") {
            current_position++;
            continue;
        }
        result = *current_position;
        break;
    }

    return  result;
}


template<typename IteratorType>
void Tokenizer<IteratorType>::check_token() {
    char next_token = get_next_token_without_spaces();
    switch (next_token) {
        case next_token == '{':
            break;
        case next_token == '}':
            break;
        case next_token == '[':
            break;
        case next_token == ']':
            break;
        case next_token == '"':
            break;
        case next_token == '-' || (next_token >= '0' && next_token <= '9'):
            break;
        case next_token == 't':
            break;
        case next_token == 'f':
            break;
    }
}
