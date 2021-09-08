#pragma once

#include <variant>

namespace json {

const auto any = [](const char in) -> bool { return true; };

const auto digit = [](const char in) -> bool {
    return in >= '0' && in <= '9';
};

const auto digit_pos = [](const char in) -> bool {
    return in >= '1' && in <= '9';
};


const auto integer = 
    parsec::seq::andThen({
        // sign
        parsec::optional(parsec::match::ch('-')),
        // digit, "01" is not a valid number
        parsec::seq::orElse({
            parsec::seq::andThen({
                parsec::match::ch_fn(digit_pos),
                parsec::match::any(parsec::match::ch_fn(digit))
            }),
            parsec::match::ch('0')
        }),
    });

const auto fraction = 
    // if a dot is found, we require the fractional part
    parsec::seq::xImplies({
        parsec::match::ch('.'),
        parsec::match::some(parsec::match::ch_fn(digit))
    });

const auto exponent = 
    parsec::seq::xImplies({
        parsec::seq::orElse({
            parsec::match::ch('e'),
            parsec::match::ch('E')
        }),
        parsec::seq::andThen({
            parsec::optional(
                parsec::seq::orElse({
                    parsec::match::ch('-'),
                    parsec::match::ch('+'),
                })
            ),
            parsec::match::some(parsec::match::ch_fn(digit))
        })
    });

const auto number = parsec::seq::andThen({
    integer,
    fraction,
    exponent
});

const auto string = parsec::seq::andThen({
    parsec::match::ch('"'),
    parsec::match::any(
        parsec::seq::orElse({
            parsec::seq::xImplies({
                parsec::match::ch('\\'),
                parsec::match::ch('"'),
            }),
            parsec::match::ch_fn([](const char in) { return in != '"' && in != '\\'; })
        })
    ),
    parsec::match::ch('"'),
});
}