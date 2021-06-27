#include <tuple>
#include <string>
#include <iostream>

using namespace std;

using Result = pair<bool, string>;

Result parseA(string str) {
  if (str.length() == 0) return make_pair(false, "");
  else if (str[0] == 'A') return make_pair(true, str.substr(1));

  return make_pair(false, str);  
}


ostream& operator<< (ostream &out, const Result &res) {
  out << "(";
  out << (get<0>(res) ? "true" : "false");

  out << ", ";
  out << get<1>(res);
  out << ")";

  return out;
}

int main() {
  const Result first = parseA("ABC");
  cout << "first: " << first << "\n";

  const Result second = parseA("ZBC");
  cout << "second: " << second << "\n";


  return 0;
}