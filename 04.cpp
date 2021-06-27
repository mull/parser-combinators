#include <tuple>
#include <string>
#include <iostream>
#include <variant>

// Inner function

using namespace std;

using Failure = string;
using Success = pair<char, string>;

using Result = variant<Success, Failure>;

auto pchar(char match) {
  return [match](const string& input) -> Result {
    if (input.length() == 0) return Failure { "no more input" };

    if (input[0] == match) {
      return Success { match, input.substr(1) };
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

int main() {
  auto parseA = pchar('A');
  const Result first = parseA("ABC");
  cout << "first: " << first << "\n";

  const Result second = parseA("ZBC");
  cout << "second: " << second << "\n";

  return 0;
}