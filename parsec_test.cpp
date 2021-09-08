#include <catch2/catch_test_macros.hpp>
#include "./parsec.hpp"

using namespace parsec;


const auto is_success = [](const Result r) -> bool {
    return std::holds_alternative<Success>(r);
};

const auto is_failure = [](const Result r) -> bool { return std::holds_alternative<Failure>(r); };

const auto result_eq = [](const Result r, std::string matched, std::string remaining) -> bool {
  if (std::holds_alternative<Failure>(r)) return false;

  const auto result = std::get<Success>(r);
  return matched == std::get<0>(result)
      && remaining == std::get<1>(result);
};


const auto matcher_A = [](const char in) { return in == 'A'; };
const auto parser_A = parsec::match::ch_fn(matcher_A);

TEST_CASE("match::ch_fn") {
  REQUIRE( is_success(parser_A("A")) );
  REQUIRE( is_failure(parser_A("B")) );
}

const auto parser_FOO = parsec::match::str("FOO");
TEST_CASE("match::str") {
  REQUIRE( is_success(parser_FOO("FOO")) );
  REQUIRE( is_failure(parser_FOO("BAR")) );
}

TEST_CASE("seq::some") {
  const auto some_FOOs = parsec::seq::some(parser_FOO);

  REQUIRE( is_failure(some_FOOs("")) );

  REQUIRE( is_success(some_FOOs("FOO")) );
  REQUIRE( is_success(some_FOOs("FOOFOO")) );
  REQUIRE( is_success(some_FOOs("FOOFOOFOO")) );

  REQUIRE( is_failure(some_FOOs("FO")) );

  REQUIRE( is_success(some_FOOs("FOOBAR")) );
  REQUIRE( is_success(some_FOOs("FOOFOOBAR")) );

  REQUIRE( is_success(some_FOOs("FOOFOOBARFOO")) );


  REQUIRE ( result_eq(some_FOOs("FOOF"), "FOO", "F") );
  REQUIRE ( result_eq(some_FOOs("FOOFOO"), "FOOFOO", "") );
  REQUIRE ( result_eq(some_FOOs("FOOFOOBARFOO"), "FOOFOO", "BARFOO") );
}


TEST_CASE("seq::any") {
  const auto any_FOOs = parsec::seq::any(parser_FOO);

  REQUIRE ( is_success(any_FOOs("")) );
  REQUIRE ( is_success(any_FOOs("F")) );
  REQUIRE ( is_success(any_FOOs("FO")) );
  REQUIRE ( is_success(any_FOOs("FOO")) );
  REQUIRE ( is_success(any_FOOs("FOOFOO")) );

  REQUIRE ( result_eq(any_FOOs(""), "", "") );
  REQUIRE ( result_eq(any_FOOs("FO"), "", "FO") );
  REQUIRE ( result_eq(any_FOOs("FOOF"), "FOO", "F") );
  REQUIRE ( result_eq(any_FOOs("FOOFOO"), "FOOFOO", "") );
}


TEST_CASE("seq::xImplies") {
  const auto xThenY = parsec::seq::xImplies({
    parsec::match::ch('x'),
    parsec::match::ch('Y')
  });

  REQUIRE ( is_success(xThenY("xY") ));
  REQUIRE ( is_success(xThenY("yY") ));

  REQUIRE ( is_failure(xThenY("xy") ));
}