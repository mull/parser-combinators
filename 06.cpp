#include <tuple>
#include <string>
#include <iostream>
#include <variant>

// orElse

using namespace std;

using Failure = string;
using Success = pair<string, string>;

using Result = variant<Success, Failure>;
using Parser = function<Result(const string&)>;

auto pchar(char match) {
  return [match](const string& input) -> Result {
    if (input.length() == 0) return Failure { "no more input" };

    if (input[0] == match) {
      const string str = string { match };
      return Success { { match }, input.substr(1) };
    }

    return Failure { "Unexpected character" };
  };
}

ostream& operator<< (ostream &out, const Result &res) {
  if (holds_alternative<Failure>(res)) {
    out << "Failure(" << get<Failure>(res) << ")";
  } else {
    const auto& s = get<Success>(res);
    out << "Success('" << get<0>(s) << "', \"" << get<1>(s) << "\")";
  }

  return out;
}

auto orElse(Parser a, Parser b) {
  return [a, b](const string& input) -> Result {
    const auto result1 = a(input);
    if (holds_alternative<Success>(result1)) return result1;

    const auto result2 = b(input);
    return result2;
  };
}

int main() {
  auto parseA = pchar('A');
  auto parseB = pchar('B');

  const auto parseAORElseB = orElse(parseA, parseB);
  std::cout << "\"AZZ\" " << parseAORElseB("AZZ") << "\n";
  std::cout << "\"BZZ\" " << parseAORElseB("BZZ") << "\n";
  std::cout << "\"CZZ\" " << parseAORElseB("CZZ") << "\n";

  return 0;
}