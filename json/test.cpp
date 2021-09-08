#include <catch2/catch_test_macros.hpp>
#include "../parsec.hpp"
#include "./json.hpp"

using namespace parsec;




const auto is_success = [](const Result r) -> bool {
    return std::holds_alternative<Success>(r);
};

const auto is_failure = [](const Result r) -> bool { return std::holds_alternative<Failure>(r); };

const auto number = json::number;

// SCENARIO("Number") {
//     GIVEN("A positive integer") {
//         THEN("it succeeds") {
//             REQUIRE ( is_success(number("1")) );
//             REQUIRE ( is_success(number("154")) );
//             REQUIRE ( is_success(number("10")) );
//             REQUIRE ( is_success(number("1234567890")) );
//         }
//     }

//     GIVEN("Zero") {
//         THEN( "it matches" ) {
//             REQUIRE ( is_success(number("0")) );
//             REQUIRE ( is_success(number("-0")) );
//         }
//     }

//     GIVEN("A negative integer") {
//         THEN( "it matches") {
//             REQUIRE ( is_success(number("-1")) );
//             REQUIRE ( is_success(number("-10")) );
//             REQUIRE ( is_success(number("-9876543210")) );
//         }
//     }

//     GIVEN("A non-numerical character") {
//         THEN ("it fails") {
//             REQUIRE( is_failure(number("-A")) );
//             REQUIRE( is_failure(number("Z")) );
//         }
//     }

//     GIVEN("A fraction") {
//         THEN ("it succeeds") {
//             REQUIRE( is_success(number("0.123")) );
//             REQUIRE( is_success(number("-0.123")) );
//             REQUIRE( is_success(number("-1248.1230")) );
//             REQUIRE( is_success(number("1248.0123")) );
//         }
//     }

//     GIVEN("An incomplete fraction") {
//         THEN ("it fails") {
//             REQUIRE( is_failure(number("-0.")) );
//         }
//     }

//     GIVEN("An exponent") {
//         THEN ("it succeeds") {
//             REQUIRE ( is_success(number("11e-24")) );
//             REQUIRE ( is_success(number("11E-24")) );
//             REQUIRE ( is_success(number("11e+24")) );
//             REQUIRE ( is_success(number("11E+24")) );
//         }

//         WHEN ("the last digit is missing") {
//             REQUIRE ( is_failure(number("11E+")) );
//             REQUIRE ( is_failure(number("11E")) );
//         }
//     }

//     GIVEN("A fraction and an exponent") {
//         THEN ("it succeeds") {
//             REQUIRE ( is_success(number("0.124E+24")) );
//             REQUIRE ( is_success(number("-0.124E-24")) );
//         }
//     }
// }

SCENARIO("String") {
    // GIVEN("An unmatched starting quotation mark") {
    //     REQUIRE( is_failure(json::string("\"")) );
    // }

    GIVEN("An empty string") {
        REQUIRE( is_success(json::string("\"\"")) );
    }

    // GIVEN("A string with content") {
    //     REQUIRE( is_success(json::string("\"some text\"")) );
    // }
// std::cout << json::string("\"\"") << "\n";
}
