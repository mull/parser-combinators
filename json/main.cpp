#include "../parsec.hpp"
// #include "./json.hpp"

#include <iostream>




int main() {
  using namespace parsec;

  parsec::Parser object;
  parsec::Parser array;

  array = [&object](const std::string in) {
    return parsec::Failure { " No good " };
  };

  object = [&array](const std::string in) {
    return array(in);
  };

  const auto res = object("foo");

  std::cout << res << "\n";

  return 0;
} 