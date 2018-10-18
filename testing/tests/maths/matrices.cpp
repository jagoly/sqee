#include "../../catch.hpp"
#include "../../Common.hpp"

#include "sqee/maths/Functions.hpp"

#include <iostream>

//============================================================================//

TEST_CASE("matrix constructor tests", "[maths]")
{
    // todo
}

//============================================================================//

TEST_CASE("matrix arithmetic operators tests", "[maths]")
{
    // todo
}

//============================================================================//

TEST_CASE("matrix function tests", "[maths]")
{
    SECTION("matrix transform tests")
    {
        const Vec3F vectorX = Vec3F(3.f, 2.f, 1.f);

        REQUIRE(maths::rotation({0.f, 0.f, 0.f}, 0.f) == approx(Mat3F()));
        REQUIRE(maths::rotation({1.f, 1.f, 1.f}, 0.f) == approx(Mat3F()));
        REQUIRE(maths::rotation({1.f, 0.f, 0.f}, -0.25f) * vectorX == approx(Vec3F(3.f, -1.f, 2.f)));
    }
}
