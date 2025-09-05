#include "Polly/Array.hpp"
#include <snitch/snitch.hpp>

using namespace Polly; // NOLINT(*-build-using-namespace)

TEST_CASE("Array basics", "[stl]")
{
    {
        auto arr = Array<int, 1>();
        static_assert(decltype(arr)::fixedSize == 1);
        REQUIRE(arr.size() == 1);
        REQUIRE(arr[0] == 0);
    }

    {
        auto arr = Array{1, 2, 3};
        static_assert(decltype(arr)::fixedSize == 3);
        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }
}