#include "Polly/Algorithm.hpp"
#include "Polly/Concepts.hpp"
#include "Polly/List.hpp"
#include "Polly/String.hpp"
#include <snitch/snitch.hpp>

using namespace Polly; // NOLINT(*-build-using-namespace)

static_assert(std::is_constructible_v<StringView, const char*>);
static_assert(std::is_constructible_v<StringView, const char (&)[1]>);

template<Concepts::StringOrStringView T>
void testTrimming()
{
    auto str = T("Hello World!");
    str.trim({});
    REQUIRE(str == "Hello World!");

    str.trimStart({'H'});
    REQUIRE(str == "ello World!");

    str.trimEnd({'!'});
    REQUIRE(str == "ello World");

    str.trimStart({'e', 'l'});
    REQUIRE(str == "o World");

    str.trimEnd({'d', 'r', 'l'});
    REQUIRE(str == "o Wo");

    str.trim({'o'});
    REQUIRE(str == " W");

    str.trim({' '});
    REQUIRE(str == "W");

    str.trim({'W'});
    REQUIRE(str.isEmpty());
    REQUIRE(str == "");

    str.trim({'s', 't', 'i', 'l', 'l', 'e', 'm', 'p', 't', 'y'});
    REQUIRE(str == "");

    const auto storedStr = "Hello World!"_s.trimmed({'H', '!', 'd', 'l', 'e'});
    str                  = T(storedStr);
    REQUIRE(str == "o Wor");
}

TEST_CASE("String trim", "[stl]")
{
    testTrimming<String>();
    testTrimming<StringView>();
}

TEST_CASE("String split", "[stl]")
{
    constexpr auto str = R"(
111111
)"_sv;

    auto splits = Polly::splitString<List<String, 8>>(str, "\n");
    REQUIRE(splits[0] == "111111");
}
