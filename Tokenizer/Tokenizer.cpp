#include "Tokenizer.h"
#include "TokenState.h"

#include <ranges>
using namespace State;

Tokenizer::Tokenizer(std::string &&json) noexcept : json_value{std::move(json)} {

}



void Tokenizer::Parse() {
    auto currChar = json_value.begin();

    TokenState stateNow = TokenState::NewToken;
    TokenState stateNext = TokenState::NewToken;

    while (currChar != json_value.end()) {
        switch (stateNow) {
            case TokenState::NewToken: {

            }
        }
    }
}





char Tokenizer::get_next_token_without_spaces() {
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



void Tokenizer::check_token() {
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
