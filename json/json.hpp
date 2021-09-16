#pragma once

#include <variant>

namespace json {

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
        || (in >= 'A' && in <= 'F')
        || (in >= 'a' && in <= 'f');
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
    parsec::match::until(
        parsec::match::ch('"'),
        parsec::match::oneOf({
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
            }),
            parsec::match::ch_fn([](const char in) { return in != '\\'; })
        })
    )
});

const auto whitespace = parsec::seq::any(
    parsec::match::oneOf({
        parsec::match::ch(' '),
        parsec::match::ch('\t'),
        parsec::match::ch('\n'),
        parsec::match::ch('\r'),
    })
);


parsec::Parser make_object(const parsec::Parser& value) {
    return parsec::seq::andThen({
        parsec::match::ch('{'),
        parsec::seq::any(
            parsec::match::oneOf({
                parsec::match::repeatedly(
                    parsec::seq::andThen({
                        parsec::seq::any(whitespace),
                        string,
                        parsec::seq::any(whitespace),
                        parsec::match::ch(':'),
                        parsec::seq::any(whitespace),
                        value,
                        parsec::seq::any(whitespace),
                    }),
                    parsec::seq::andThen({
                        parsec::seq::any(whitespace),
                        parsec::match::ch(','),
                        parsec::seq::any(whitespace),
                    })
                ),
                parsec::seq::any(whitespace)
            })
        ),
        parsec::match::ch('}'),
    });
}

parsec::Parser make_array(const parsec::Parser& value) {
    return parsec::seq::andThen({
        parsec::match::ch('['),
        parsec::seq::any(
            parsec::match::oneOf({
                parsec::match::repeatedly(
                    parsec::seq::andThen({
                        parsec::seq::any(whitespace),
                        value,
                        parsec::seq::any(whitespace)
                    }),
                    parsec::seq::andThen({
                        parsec::seq::any(whitespace),
                        parsec::match::ch(','),
                        parsec::seq::any(whitespace),
                    })
                ),
                parsec::seq::any(whitespace)
            })
        ),
        parsec::match::ch(']')
    });
}


parsec::Parser parser() {
    using namespace parsec;

    // We need a lambda here to keep obj and array in memory
    // The circular dependencies between obj<>array<>value are tricky
    return [](const std::string in) {
        Parser obj;
        Parser array;

        Parser value = match::oneOf({
            string,
            number,
            [&obj](const auto in) { return obj(in); },
            [&array](const auto in) { return array(in); }
        });
        obj = make_object(value);
        array = make_array(value);

        return value(in);
    };
}

} // namespace json

