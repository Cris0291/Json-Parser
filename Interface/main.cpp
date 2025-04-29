#include <iostream>
#include "C:/Users/cris0/OneDrive/Escritorio/Json/Parse/Tokenizer/Tokenizer.h"


int main() {
    std::string json {};
    std::cout << "Please enter some text in json format" << std::endl;

    std::cin >> json;

    Tokenizer tokenizer {std::move(json)};
    std::vector<State::Token> tokens = tokenizer.Parse();

    for (auto token: tokens) {
        std::cout << "Token value: " << token.value << std::endl;
    }
}


