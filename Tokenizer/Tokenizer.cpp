module Tokenizer;
using namespace std;

template<typename InputIterator>
Tokenizer<InputIterator>::Tokenizer(std::string&& json) noexcept{
    json_value = std::move(json);
    current_position = std::begin()
};

