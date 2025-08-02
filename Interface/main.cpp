#include <iostream>
#include "Tokenizer.h"



int main() {
    std::cout << "Please enter some text in json format" << std::endl;

    Tokenizer tokenizer {std::filesystem::path{"C:\\Users\\cris0\\OneDrive\\Escritorio\\stest.json"}};
    auto tokens = tokenizer.Parse();
    JsonDescriptor descriptor {};
    descriptor.createJsonMap(tokens);

}


