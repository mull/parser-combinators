#include <tuple>
#include <string>
#include <iostream>


using namespace std;

using Result = pair<string, string>;

Result pchar(char match, string input) {
  if (input.length() == 0) return make_pair("No more input", "");
  else {
    if (input[0] == match) {
      return make_pair("Found match", input.substr(1));
    } else {
      return make_pair("Found unexpected char", input);
    }
  }
}

ostream& operator<< (ostream &out, const Result &res) {
  out << "(";
  out << get<0>(res);
  out << ", ";
  out << get<1>(res);
  out << ")";

  return out;
}

int main() {
  const Result first = pchar('A', "ABC");
  cout << "first: " << first << "\n";

  const Result second = pchar('A', "ZBC");
  cout << "second: " << second << "\n";

  return 0;
}