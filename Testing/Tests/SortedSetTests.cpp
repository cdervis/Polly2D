#include "Polly/Array.hpp"
#include "Polly/SortedSet.hpp"
#include <snitch/snitch.hpp>

using namespace Polly; // NOLINT(*-build-using-namespace)

TEST_CASE("SortedSet basics", "[stl]")
{
    auto set = SortedSet<int>();
    REQUIRE(set.isEmpty());
    REQUIRE(set.size() == 0);

    set.add(1);
    REQUIRE_FALSE(set.isEmpty());
    REQUIRE(set.size() == 1);
}
