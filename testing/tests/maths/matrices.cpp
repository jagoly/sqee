#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Functions.hpp>

#include "Common.hpp"

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

        REQUIRE_THAT(maths::rotation({0.f, 0.f, 0.f}, 0.f), WithinAbs(Mat3F()));
        REQUIRE_THAT(maths::rotation({1.f, 1.f, 1.f}, 0.f), WithinAbs(Mat3F()));
        REQUIRE_THAT(maths::rotation({1.f, 0.f, 0.f}, -0.25f) * vectorX, WithinAbs(Vec3F(3.f, -1.f, 2.f)));
    }
}
