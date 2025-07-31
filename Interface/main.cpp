#include <iostream>


#include "Tokenizer.h"


int main() {
    std::cout << "Please enter some text in json format" << std::endl;

    Tokenizer tokenizer {std::filesystem::path{"C:\\Users\\cris0\\OneDrive\\Escritorio\\test2.json"}};
    auto tokens = tokenizer.Parse();
    tokenizer.CreateTokens();

    for (auto token: tokens) {
        std::cout << "Token value: " << token.value << std::endl;
    }
}


