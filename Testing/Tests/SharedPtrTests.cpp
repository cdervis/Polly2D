#include "Polly/SharedPtr.hpp"
#include <snitch/snitch.hpp>

using namespace Polly; // NOLINT(*-build-using-namespace)

TEST_CASE("SharedPtr basics", "[stl]")
{
    auto shared_ptr = SharedPtr<int>();
    REQUIRE(shared_ptr.get() == nullptr);
    REQUIRE(shared_ptr == nullptr);
    REQUIRE_FALSE(shared_ptr);

    shared_ptr = nullptr;
    REQUIRE(shared_ptr.get() == nullptr);
    REQUIRE(shared_ptr == nullptr);
    REQUIRE_FALSE(shared_ptr);

    auto shared_ptr2 = SharedPtr(shared_ptr);
    REQUIRE(shared_ptr2.get() == nullptr);
    REQUIRE(shared_ptr2 == nullptr);
    REQUIRE_FALSE(shared_ptr2);
}
