//
// Created by cris0 on 5/2/2025.
//

#ifndef JSONOBJECT_H
#define JSONOBJECT_H
#include <string>
#include <unordered_map>
#include <vector>

#include "Tokenizer/TokenState.h"


class JsonDescriptor {
public:
    template<typename T>
    T get(const std::string& key) const;
private:
    std::unordered_map<std::string, std::string> json_map;
    void createJsonMap(std::vector<State::Token>);
    int parseJsonToken(State::Token);
};



#endif //JSONOBJECT_H
