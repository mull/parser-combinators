#include <tuple>
#include <string>
#include <iostream>
#include <variant>

// Combining two parsers in sequence

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

auto andThen(Parser a, Parser b) {
  return [a, b](const string& input) -> Result {
    const auto result1 = a(input);

    if (holds_alternative<Failure>(result1)) return result1;
    const auto& val_1 = get<Success>(result1);
    const auto result2 = b(get<1>(val_1));

    if (holds_alternative<Failure>(result2)) return result2;
    const auto& val_2 = get<Success>(result2);

    const string str1 = get<0>(val_1);
    const string str2 = get<0>(val_2);

    const string both = str1 + str2;
    return Success { both, get<1>(val_2) };
  };
}

int main() {
  auto parseA = pchar('A');
  auto parseB = pchar('B');

  const auto parseAThenB = andThen(parseA, parseB);
  std::cout << "\"ABC\" " << parseAThenB("ABC") << "\n";
  std::cout << "\"ZBC\" " << parseAThenB("ZBC") << "\n";
  std::cout << "\"AZC\" " << parseAThenB("AZC") << "\n";

  return 0;
}