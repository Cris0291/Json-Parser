#include <iostream>

#include "test2.h"
#include "Tokenizer.h"



int main() {
    std::cout << "Please enter some text in json format" << std::endl;

    Tokenizer tokenizer {std::filesystem::path{"C:\\Users\\cris0\\OneDrive\\Escritorio\\s.json"}};
    auto tokens = tokenizer.Parse();
    JsonDescriptor descriptor {};
    descriptor.createJsonMap(tokens);

     auto data = from_json<ljson::obj>(descriptor);

    std::cout << data.numberInteger << "\n"
    << data.sval  << "\n"
    << data.numberFloat << "\n"
    << data.booleanTrue << "\n"
    << data.booleanFalse << std::endl;

    if (!data.nullValue.has_value())
        std::cout << "nullValue is null\n";

    for (auto item : data.array) {
        std::cout << item.value << std::endl;
    }

    std::cout << "Nested properties" << std::endl;

    std::cout << data.object.nestedString << std::endl;

    for (auto item : data.object.nestedArray) {
        std::cout << item << std::endl;
    }

    std::cout << data.object.deepObject.deep << std::endl;
    std::cout << data.object.deepObject.deeper.value << std::endl;

    std::cout << data.emptyObject.value<< std::endl;

    for (int i {}; i < 7; i++) {
        std::cout << data.emptyArray[i] << std::endl;
    }

}


