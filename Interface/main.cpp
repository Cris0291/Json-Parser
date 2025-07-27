#include <iostream>
#include "Tokenizer/Tokenizer.h"


int main() {
    std::cout << "Please enter some text in json format" << std::endl;

    std::string json;
    std::getline(std::cin, json);
    Tokenizer tokenizer {std::move(json)};
    std::vector<State::Token> tokens = tokenizer.Parse();
std::cout << "here fine 3" << std::endl;
    for (auto token: tokens) {
        std::cout << "Token value: " << token.value << std::endl;
    }
}


