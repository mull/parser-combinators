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
        parsec::match::oneOf({
            parsec::seq::andThen({
                parsec::match::ch_fn(digit_pos),
                parsec::seq::any(parsec::match::ch_fn(digit))
            }),
            parsec::match::ch('0')
        }),
    });

const auto fraction = 
    // if a dot is found, we require the fractional part
    parsec::seq::xImplies({
        parsec::match::ch('.'),
        parsec::seq::some(parsec::match::ch_fn(digit))
    });

const auto exponent = 
    parsec::seq::xImplies({
        parsec::match::oneOf({
            parsec::match::ch('e'),
            parsec::match::ch('E')
        }),
        parsec::seq::andThen({
            parsec::optional(
                parsec::match::oneOf({
                    parsec::match::ch('-'),
                    parsec::match::ch('+'),
                })
            ),
            parsec::seq::some(parsec::match::ch_fn(digit))
        })
    });

const auto number = parsec::seq::andThen({
    integer,
    fraction,
    exponent
});

const auto hex = parsec::match::ch_fn([](const char in) -> bool {
    return (in >= '0' && in <= '9')
        || (in >= 'A' && in <= 'F');
});

const auto unicode_str = parsec::seq::andThen({
    parsec::match::ch('u'),
    hex,
    hex,
    hex,
    hex
});

const auto string = parsec::seq::andThen({
    parsec::match::ch('"'),
    parsec::seq::any(
        parsec::match::oneOf({
            parsec::match::ch_fn([](const char in) { return in != '"' && in != '\\'; }),
            parsec::seq::andThen({
                parsec::match::ch('\\'),
                parsec::match::oneOf({
                    parsec::match::ch('"'),
                    parsec::match::ch('\\'), // reverse solidus
                    parsec::match::ch('/'), // solidus
                    parsec::match::ch('b'), // backspace
                    parsec::match::ch('f'), // formfeed
                    parsec::match::ch('n'), // formfeed
                    parsec::match::ch('r'), // formfeed
                    parsec::match::ch('t'), // formfeed
                    unicode_str,
                })
            })
        })
    )
    ,
    parsec::match::ch('"'),
});
}