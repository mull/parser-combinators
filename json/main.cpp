#include "../parsec.hpp"
#include "./json.hpp"

#include <iostream>




int main() {

  // std::cout << "json_null(NULL) " << json_null("NULL") << "\n";
  // std::cout << "json_null(NULl) " << json_null("NULl") << "\n";

  std::cout << "json_string(\"\") " << json::string("\"\"") << "\n";
  std::cout << "json_string(\"a\") " << json::string("\"a\"") << "\n";
  std::cout << "json_string(\"asd\") " << json::string("\"asd\"") << "\n";

  std::cout << "json_object({}) " << json::object("{}") << "\n";
  std::cout << "json_object({]) " << json::object("{]") << "\n";

  std::cout << "json_object([}) " << json::object("{]") << "\n";
  std::cout << "json_object({\"key\": \"value\"}) " << json::object("{\"key\": \"value\"") << "\n";

  return 0;
} 