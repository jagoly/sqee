#include "../../catch.hpp"
#include "../../Common.hpp"

//============================================================================//

TEST_CASE("quaternion constructor tests", "[maths]")
{
    SECTION("eular constructor")
    {
        REQUIRE(QuatF(0.0f, 0.0f, 0.0f) == Approx(QuatF(0.0f, 0.0f, 0.0f, 1.0f)));
        REQUIRE(QuatF(1.0f, -1.0f, 1.0f) == Approx(QuatF(0.0f, 0.0f, 0.0f, 1.0f)));
        REQUIRE(QuatF(-1.0f, 1.0f, -1.0f) == Approx(QuatF(0.0f, 0.0f, 0.0f, 1.0f)));

        REQUIRE(QuatF(0.25f, 0.0f, 0.0f) == Approx(QuatF(0.707107f, 0.0f, 0.0f, 0.707107f)));
        REQUIRE(QuatF(-0.5f, 0.0f, 0.0f) == Approx(QuatF(-1.0f, 0.0f, 0.0f, 0.0f)));
        REQUIRE(QuatF(0.75f, 0.0f, 0.0f) == Approx(QuatF(0.707107f, 0.0f, 0.0f, -0.707107f)));

        REQUIRE(QuatF(0.0f, 0.25f, 0.0f) == Approx(QuatF(0.0f, 0.707107f, 0.0f, 0.707107f)));
        REQUIRE(QuatF(0.0f, -0.5f, 0.0f) == Approx(QuatF(0.0f, -1.0f, 0.0f, 0.0f)));
        REQUIRE(QuatF(0.0f, 0.75f, 0.0f) == Approx(QuatF(0.0f, 0.707107f, 0.0f, -0.707107f)));

        REQUIRE(QuatF(0.0f, 0.0f, 0.25f) == Approx(QuatF(0.0f, 0.0f, 0.707107f, 0.707107f)));
        REQUIRE(QuatF(0.0f, 0.0f, -0.5f) == Approx(QuatF(0.0f, 0.0f, -1.0f, 0.0f)));
        REQUIRE(QuatF(0.0f, 0.0f, 0.75f) == Approx(QuatF(0.0f, 0.0f, 0.707107f, -0.707107f)));
    }

    SECTION("matrix constructor")
    {
        REQUIRE(QuatF(Mat3F()) == Approx(QuatF(0.0f, 0.0f, 0.0f, 1.0f)));
    }
}

//============================================================================//

TEST_CASE("quaternion arithmetic operators tests", "[maths]")
{
    SECTION("quaternion / scalar binary operators")
    {
        REQUIRE(QuatF(1.0f, 2.0f, 3.0f, 4.0f) * 2.0f == Approx(QuatF(2.0f, 4.0f, 6.0f, 8.0f)));
        REQUIRE(QuatF(1.0f, 2.0f, 3.0f, 4.0f) / 2.0f == Approx(QuatF(0.5f, 1.0f, 1.5f, 2.0f)));
    }

    SECTION("quaternion / quaternion multiplication")
    {
        REQUIRE(QuatF() * QuatF(1.f, 0.f, 0.f, 0.f) == Approx(QuatF(1.f, 0.f, 0.f, 0.f)));
        REQUIRE(QuatF(0.f, 0.f, 1.f, 0.f) * QuatF() == Approx(QuatF(0.f, 0.f, 1.f, 0.f)));
    }

    SECTION("quaternion / vector multiplication")
    {
        REQUIRE(QuatF(0.5f, 0.0f, 0.0f) * Vec3F(1.f, 2.f, 3.f) == Approx(Vec3F(1.f, -2.f, -3.f)));
        REQUIRE(QuatF(0.0f, 0.5f, 0.0f) * Vec3F(1.f, 2.f, 3.f) == Approx(Vec3F(-1.f, 2.f, -3.f)));
        REQUIRE(QuatF(0.0f, 0.0f, 0.5f) * Vec3F(1.f, 2.f, 3.f) == Approx(Vec3F(-1.f, -2.f, 3.f)));
    }
}

//============================================================================//

TEST_CASE("quaternion function tests", "[maths]")
{
//    SECTION("quaternion / mat")
}
