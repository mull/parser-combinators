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

namespace match {
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

  Parser endless(const Parser parser) {
    return [parser](const string input) -> Result {
      string result {""};
      string remaining {input};
      for (const char c : input) {
        const string cand { c };
        if (std::holds_alternative<Success>(parser(cand))) {
          result.append(cand);
          remaining = remaining.substr(1);
        } else {
          break;
        }
      }

      if (result.length() > 0) { // TODO: Yeah?
        return Success { result, remaining };
      }
      return Failure { "No match for endless" };
    };
  }
}
}