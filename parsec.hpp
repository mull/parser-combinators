#pragma once

#include <variant>
#include <string>
#include <tuple>
#include <concepts>
#include <vector>
#include <iostream>
#include <type_traits>
#include <optional>


namespace parsec {
using namespace std;

using Failure = string;
using Success = pair<string, string>;

using Result = variant<Success, Failure>;

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
      if (input.length() == 0) return Failure { "ch_fn: No input" };
      if (m(input[0])) return Success { {input[0]}, input.substr(1) };

      return Failure { "" };
    };
  }

  Parser ch(const char match) {
    return [match](const string input) -> Result {
      if (input.length() == 0) return Failure {"ch: No input"};
      if (input[0] == match) {
        return Success { {match}, input.substr(1) };
      }
      return Failure { std::string("ch: No match for '") + std::string(1, match) };
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
      if (input.length() < match.length()) return Failure {"ch: No input"};

      const string result = input.substr(0, match.length());
      if (result == match) {
        return Success { result, input.substr(match.length()) };
      }

      return Failure { "No match" };
    };
  }

  Parser oneOf(const std::vector<Parser> parsers) {
    return [parsers](const string input) -> Result {
      for (const auto p : parsers) {
        const auto p_res = p(input);
        if (std::holds_alternative<Success>(p_res)) return p_res;
      }

      return Failure { "No alternative worked." };
    };
  }

  Parser until(const Parser breakPoint, const Parser untilThen) {
    return [breakPoint, untilThen](const string input) -> Result {
      std::string result {""};
      auto remaining { input };

      while (true) {
        if (remaining.length() == 0) { return Failure { "until: No more input" }; }

        const auto b_res = breakPoint(remaining);
        if (std::holds_alternative<Failure>(b_res)) {
          const auto u_res = untilThen(remaining);
          if (std::holds_alternative<Failure>(u_res)) {
            return u_res;
          }

          const auto u = std::get<Success>(u_res);
          result.append(std::get<0>(u));
          remaining = remaining.substr(std::get<0>(u).length());
        } else {
          const auto b = std::get<Success>(b_res);
          result.append(std::get<0>(b));
          return Success { result, remaining.substr(std::get<0>(b).length()) };
        }
      }
    };
  }

  Parser repeatedly(const Parser matchOn, std::optional<Parser> joinedBy = std::nullopt) {
    return [matchOn, joinedBy](const string input) -> Result {
      std::string remaining { input };
      std::string match { "" };

      std::string appendage { "" };

      while (true) {
        if (remaining.length() == 0) break;

        const auto m_res = matchOn(remaining);
        if (std::holds_alternative<Failure>(m_res)) break;

        const auto m = std::get<Success>(m_res);
        match.append(appendage);
        appendage = "";
        match.append(std::get<0>(m));
        remaining = std::get<1>(m);

        if (joinedBy) {
          const auto j_res = joinedBy.value()(remaining);
          if (std::holds_alternative<Failure>(j_res)) break;

          const auto j = std::get<Success>(j_res);
          // match.append(std::get<0>(j));
          appendage = std::get<0>(j);
          remaining = std::get<1>(j);
        }
      }

      if (match.length() == 0) return Failure { "repatedly: no match" };
      if (appendage.length() != 0) return Failure { "repeatedly: dangling appendage" };
      return Success { match, remaining };
    };
  }
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
        // TODO: Use Maybe instead?
        if (std::get<0>(s).length() == 0) break;

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

  TODO: xImplies and oneOf are not usable together if xImplies is the first argument (it returns Success even if it matches nothing)
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

