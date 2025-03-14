#include <string>
#include <iterator>

export module Tokenizer;

export template<typename IteratorType>
class Tokenizer{
  public:
    Tokenizer(std::string&& json) noexcept;
    void parser();
  private:
    std::string json_value;
    typename std::iterator_traits<IteratorType>::value_type current_position;
};