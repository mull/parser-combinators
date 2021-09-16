#include <catch2/catch_test_macros.hpp>
#include "../parsec.hpp"
#include "./json.hpp"

using namespace parsec;




const auto is_success = [](const Result r) -> bool {
    return std::holds_alternative<Success>(r);
};

const auto is_failure = [](const Result r) -> bool { return std::holds_alternative<Failure>(r); };
const auto number = json::number;
const auto parse_json = json::parser();

SCENARIO("Number") {
    GIVEN("A positive integer") {
        THEN("it succeeds") {
            REQUIRE ( is_success(number("1")) );
            REQUIRE ( is_success(number("154")) );
            REQUIRE ( is_success(number("10")) );
            REQUIRE ( is_success(number("1234567890")) );
        }
    }

    GIVEN("Zero") {
        THEN( "it matches" ) {
            REQUIRE ( is_success(number("0")) );
            REQUIRE ( is_success(number("-0")) );
        }
    }

    GIVEN("A negative integer") {
        THEN( "it matches") {
            REQUIRE ( is_success(number("-1")) );
            REQUIRE ( is_success(number("-10")) );
            REQUIRE ( is_success(number("-9876543210")) );
        }
    }

    GIVEN("A non-numerical character") {
        THEN ("it fails") {
            REQUIRE( is_failure(number("-A")) );
            REQUIRE( is_failure(number("Z")) );
        }
    }

    GIVEN("A fraction") {
        THEN ("it succeeds") {
            REQUIRE( is_success(number("0.123")) );
            REQUIRE( is_success(number("-0.123")) );
            REQUIRE( is_success(number("-1248.1230")) );
            REQUIRE( is_success(number("1248.0123")) );
        }
    }

    GIVEN("An incomplete fraction") {
        THEN ("it fails") {
            REQUIRE( is_failure(number("-0.")) );
        }
    }

    GIVEN("An exponent") {
        THEN ("it succeeds") {
            REQUIRE ( is_success(number("11e-24")) );
            REQUIRE ( is_success(number("11E-24")) );
            REQUIRE ( is_success(number("11e+24")) );
            REQUIRE ( is_success(number("11E+24")) );
        }

        WHEN ("the last digit is missing") {
            REQUIRE ( is_failure(number("11E+")) );
            REQUIRE ( is_failure(number("11E")) );
        }
    }

    GIVEN("A fraction and an exponent") {
        THEN ("it succeeds") {
            REQUIRE ( is_success(number("0.124E+24")) );
            REQUIRE ( is_success(number("-0.124E-24")) );
        }
    }
}

SCENARIO("String") {
    const auto wrap = [](const std::string in) { 
        return "\"" + in + "\"";
    };


    GIVEN("An unmatched starting quotation mark") {
        REQUIRE( is_failure(json::string("\"")) );
    }

    GIVEN("An empty string") {
        REQUIRE( is_success(json::string("\"\"")) );
    }

    GIVEN("A string with content") {
        REQUIRE( is_success(json::string("\"some text\"")) );
    }

    GIVEN("All escape characters") {
        REQUIRE( is_success(json::string(wrap("\\\""))) );
        REQUIRE( is_success(json::string(wrap("\\\\"))) );
        REQUIRE( is_success(json::string(wrap("\\/"))) );
        REQUIRE( is_success(json::string(wrap("\\b"))) );
        REQUIRE( is_success(json::string(wrap("\\f"))) );
        REQUIRE( is_success(json::string(wrap("\\n"))) );
        REQUIRE( is_success(json::string(wrap("\\r"))) );
        REQUIRE( is_success(json::string(wrap("\\t"))) );
    }

    GIVEN("Unicode") {
        REQUIRE( is_failure(json::string(wrap("\\u"))) );
        REQUIRE( is_failure(json::string(wrap("\\u1"))) );
        REQUIRE( is_failure(json::string(wrap("\\u11"))) );
        REQUIRE( is_failure(json::string(wrap("\\u111"))) );
        REQUIRE( is_success(json::string(wrap("\\u1111"))) );
        REQUIRE( is_success(json::string(wrap("\\uADED"))) );
        REQUIRE( is_success(json::string(wrap("\\uBEEF"))) );

        REQUIRE( is_success(json::string(wrap("\\udead"))) );
        REQUIRE( is_success(json::string(wrap("\\ufeec"))) );
        REQUIRE( is_success(json::string(wrap("\\uDeaD"))) );
    }

    GIVEN("A few random strings") {
        REQUIRE( is_success(json::string(wrap("Now\\nis\\btheWinter\\r\\tOfOur\"discontent"))) );
    }

    GIVEN("A string with an incorrect terminator") {
        // It is not up to json::string to match anything outside the actual string
        // So this is still successful
        REQUIRE( is_success(json::string(wrap("This is inside\"Suddenly outside"))) );
    }
}


TEST_CASE("whitespace") {
    REQUIRE( is_success(json::whitespace(" ")) );
    REQUIRE( is_success(json::whitespace("\t")) );
    REQUIRE( is_success(json::whitespace("\n")) );
    REQUIRE( is_success(json::whitespace("\r")) );
}

TEST_CASE("object") {
    const auto parser = json::parser();

    GIVEN("Empty object") {
        REQUIRE( is_success(parser("{}")) );
        REQUIRE( is_success(parser("{ }")) );
        REQUIRE( is_success(parser("{\t}")) );
        REQUIRE( is_success(parser("{\t\n}")) );
        REQUIRE( is_success(parser("{\t\r\n}")) );
    }

    GIVEN("Key") {
        REQUIRE( is_success(parser("{ \"foo\": 1}")) );
        REQUIRE( is_success(parser("{ \"foo\" : 1}")) );
        REQUIRE( is_success(parser("{\"foo\":    1}")) );
        REQUIRE( is_success(parser("{\"key\": 1337}")));
    }

    GIVEN("Value") {
        WHEN("Is a string") {
            REQUIRE( is_success(parser("{ \"foo\": \"bar\"}")) );
        }

        WHEN("Is a number") {
            REQUIRE( is_success(parser("{ \"foo\": -421.123e-124}")) );
        }

        WHEN("Is an object") {
            REQUIRE( is_success(parser("{ \"foo\": {\"bar\": \"foobar\"}}")) );
        }
    }

    GIVEN("Multiple keys") {
        REQUIRE( is_success(parser("{\"foo\": \"bar\", \"bar\": \"foo\"}")) );
    }
}


SCENARIO("Array") {
    GIVEN("Empty array") {
        REQUIRE( is_success(parse_json("[]")) );
        REQUIRE( is_success(parse_json("[ ]")) );
        REQUIRE( is_success(parse_json("[\t]")) );
        REQUIRE( is_success(parse_json("[\t\n]")) );
        REQUIRE( is_success(parse_json("[\t\r\n]")) );
    }

    GIVEN("Value") {
        REQUIRE( is_success(parse_json("[1]")) );
        REQUIRE( is_success(parse_json("[1.23e-1]")) );
        REQUIRE( is_success(parse_json("[\"a string\"]")) );
        REQUIRE( is_success(parse_json("[{\"key\": \"value\"}]")) );
        REQUIRE( is_success(parse_json("[[[[]]]]")) );
        REQUIRE( is_success(parse_json("[[[[123]]]]")) );
    }
}