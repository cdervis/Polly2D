#include "Polly/Linalg.hpp"
#include "Polly/LinalgOps.hpp"
#include <snitch/snitch.hpp>

using namespace Polly;

TEST_CASE("Vec2", "[math]")
{
    REQUIRE(Vec2() == Vec2(0, 0));

    {
        auto v = Vec2(1, 2);
        REQUIRE(v.x == 1.0f);
        REQUIRE(v.y == 2.0f);
    }
}

TEST_CASE("Vec3", "[math]")
{
    REQUIRE(Vec3() == Vec3(0, 0, 0));

    {
        constexpr auto v = Vec3(1, 2, 3);
        REQUIRE(v.x == 1.0f);
        REQUIRE(v.y == 2.0f);
        REQUIRE(v.z == 3.0f);
    }
}

TEST_CASE("Vec4", "[math]")
{
    REQUIRE(Vec4() == Vec4(0, 0, 0, 0));

    {
        constexpr auto v = Vec4(1, 2, 3, 4);
        REQUIRE(v.x == 1.0f);
        REQUIRE(v.y == 2.0f);
        REQUIRE(v.z == 3.0f);
        REQUIRE(v.w == 4.0f);
    }
}

TEST_CASE("Matrix", "[math]")
{
    REQUIRE(Matrix() == Matrix(Vec4(1, 0, 0, 0), Vec4(0, 1, 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1)));
}
