#include <iostream>

#include "test.h"
#include "Tokenizer.h"



int main() {
    std::cout << "Please enter some text in json format" << std::endl;

    Tokenizer tokenizer {std::filesystem::path{"C:\\Users\\cris0\\OneDrive\\Escritorio\\test2.json"}};
    auto tokens = tokenizer.Parse();
    JsonDescriptor descriptor {};
    descriptor.createJsonMap(tokens);

     //auto data = from_json<example::Root>(descriptor);

    //std::cout << data.logs[0].messages[0] << std::endl;

}


