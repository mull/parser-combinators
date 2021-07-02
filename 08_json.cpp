#include "parsec.hpp"

#include <iostream>

const auto json_null = parsec::match::str("NULL");
const auto json_string = parsec::seq::andThen({
  parsec::match::ch('"'),
  parsec::seq::orElse({
    parsec::match::ch('"'),
    parsec::seq::andThen({
      parsec::seq::endless(parsec::match::alpha()),
      parsec::match::ch('"')
    })
  })
});

const auto json_object = parsec::seq::andThen({
  parsec::match::ch('{'),
  parsec::seq::orElse({
    parsec::match::ch('}'), // empty object
    parsec::seq::andThen({
      json_string,
      parsec::match::str(": "),
      json_string
    })
  })
});
const auto json_array = parsec::match::str("[]");


std::ostream& operator<< (std::ostream &out, const parsec::Result &res) {
  if (holds_alternative<parsec::Failure>(res)) {
    out << "Failure(" << get<parsec::Failure>(res) << ")";
  } else {
    const auto& s = get<parsec::Success>(res);
    out << "Success('" << get<0>(s) << "', \"" << get<1>(s) << "\")";
  }

  return out;
}

int main() {

  // std::cout << "json_null(NULL) " << json_null("NULL") << "\n";
  // std::cout << "json_null(NULl) " << json_null("NULl") << "\n";

  std::cout << "json_string(\"\") " << json_string("\"\"") << "\n";
  std::cout << "json_string(\"a\") " << json_string("\"a\"") << "\n";
  std::cout << "json_string(\"asd\") " << json_string("\"asd\"") << "\n";

  std::cout << "json_object({}) " << json_object("{}") << "\n";
  std::cout << "json_object({]) " << json_object("{]") << "\n";

  std::cout << "json_object([}) " << json_object("{]") << "\n";
  std::cout << "json_object({\"key\": \"value\"}) " << json_object("{\"key\": \"value\"") << "\n";

  return 0;
} 