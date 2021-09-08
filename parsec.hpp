#pragma once

#include <variant>
#include <string>
#include <tuple>
#include <concepts>
#include <vector>
#include <iostream>


namespace parsec {
using namespace std;

using Failure = string;
using Success = pair<string, string>;

using Result = variant<Success, Failure>;
// TODO: I think it ought to take a char
using Parser = function<Result(const string)>;
  using Matcher = function<bool(const char in)>;


std::ostream& operator<< (std::ostream &out, const parsec::Result &res) {
  if (holds_alternative<parsec::Failure>(res)) {
    out << "Failure(" << get<parsec::Failure>(res) << ")";
  } else {
    const auto& s = get<parsec::Success>(res);
    out << "Success('" << get<0>(s) << "', \"" << get<1>(s) << "\")";
  }

  return out;
}

Parser optional(const Parser p) {
  return [p](const string input) -> Result {
    const auto res = p(input);
    if (std::holds_alternative<Failure>(res)) {
      return Success { "", input };
    }
    return res;
  };
};

namespace match {

  Parser ch_fn(const Matcher m) {
    return [m](const string input) -> Result {
      if (input.length() == 0) return Failure { "No input" };
      if (m(input[0])) return Success { {input[0]}, input.substr(1) };

      return Failure { "" };
    };
  }

  Parser ch(const char match) {
    return [match](const string input) -> Result {
      if (input.length() == 0) return Failure {"No input"};
      if (input[0] == match) {
        return Success { {match}, input.substr(1) };
      }

      return Failure { "NEIN" };
    };
  }

  Parser alpha() {
    return [](const string input) -> Result {
      if (input.length() > 0 && isalpha(input[0])) {
        return Success { { input[0] }, input.substr(1) };
      }

      return Failure { "Expected alphanumeric character" };
    };
  }

  Parser str(const string match) { 
    // TODO: static_assert(match.length() > 0); if possible?

    return [match](const string input) -> Result {
      if (input.length() < match.length()) return Failure {"No input"};

      const string result = input.substr(0, match.length());
      if (result == match) {
        return Success { result, input.substr(match.length()) };
      }

      return Failure { "No match" };
    };
  };
}

namespace seq {
  // TODO: Use array/initializer_list?
  Parser andThen(const std::vector<Parser> parsers) {
    return [parsers](const string input) -> Result {
      string result {""};
      string remaining {input};
      for (const auto p : parsers) {
        const auto p_res = p(remaining);
        if (std::holds_alternative<Failure>(p_res)) return p_res;
        const auto p_succ = std::get<Success>(p_res); 
        result.append(std::get<0>(p_succ));
        remaining = std::get<1>(p_succ);
      }

      return Success { result, remaining };
    };
  }

  Parser orElse(const std::vector<Parser> parsers) {
    return [parsers](const string input) -> Result {
      for (const auto p : parsers) {
        const auto p_res = p(input);
        if (std::holds_alternative<Success>(p_res)) return p_res;
      }

      return Failure { "No alternative worked." };
    };
  }

  Parser some(const Parser p) {
    return [p](const string input) -> Result {
      string result {""};
      string remaining {input};

      while (true) {
        const auto p_res = p(remaining);

        if (std::holds_alternative<Failure>(p_res)) break;

        const auto s = std::get<Success>(p_res);
        result.append(std::get<0>(s));
        remaining = remaining.substr(std::get<0>(s).length());
      }

      if (result.length() == 0) {
        return Failure { "No result for some" };
      }
      return Success { result, remaining };
    };
  }

  Parser any(const Parser p) {
    return [p](const string input) -> Result {
      if (input.length() == 0) return Success { "", "" };

      string result {""};
      string remaining {input};

      while (true) {
        const auto p_res = p(remaining);

        if (std::holds_alternative<Failure>(p_res)) break;

        const auto s = std::get<Success>(p_res);
        result.append(std::get<0>(s));
        remaining = remaining.substr(std::get<0>(s).length());
      }

      return Success { result, remaining };
    };
  }

  /*
  1 | 2 | xnor |
  t | f | f |
  t | t | t |
  f | f | t |
  f | t | t |

  If the first parser succeeds then the second one must also succeed.
  If the first one fails, then the second one does not matter.
  */
 Parser xImplies(const std::array<Parser, 2> parsers) {
    return [parsers](const string input) -> Result {
      const auto f_res = parsers[0](input);

      if (std::holds_alternative<Failure>(f_res)) return Success { "", input };
      const auto f = std::get<Success>(f_res);
      auto build { std::get<0>(f) };

      const auto s_res = parsers[1](input.substr(build.length()));
      // TODO: Combined error message somehow
      if (std::holds_alternative<Failure>(s_res)) return s_res;
      const auto s = std::get<Success>(s_res);
      build.append(std::get<0>(s));

      return Success { build, std::get<1>(s) };
    };
 }
}

}

