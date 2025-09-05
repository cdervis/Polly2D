#include "Polly/Any.hpp"
#include <snitch/snitch.hpp>

using namespace Polly; // NOLINT(*-build-using-namespace)

TEST_CASE("Any construction", "[stl]")
{
    auto a = Any();
    REQUIRE_FALSE(a);

    REQUIRE(a.type() == AnyType::None);
    REQUIRE_THROWS_MATCHES(a.get<int>(), Error());
    REQUIRE(a.getOr(10.0f) == 10.0f);
    REQUIRE_FALSE(a.tryGet<int>());

    auto b = Any();
    REQUIRE(a == b);

    a = Any(10);
    REQUIRE(a);
    REQUIRE(a.type() == AnyType::Int);
    REQUIRE(a.get<int>() == 10);
    REQUIRE(a.getOr(5) == 10);
    REQUIRE(a.tryGet<int>().valueOr(5) == 10);
    REQUIRE(a != b);

    b = 10;
    REQUIRE(b);
    REQUIRE(b.type() == AnyType::Int);
    REQUIRE(a == b);
}
