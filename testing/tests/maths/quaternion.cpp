#include <sqee/maths/Quaternion.hpp>

#include "Common.hpp"

//============================================================================//

TEST_CASE("quaternion constructor tests", "[maths]")
{
    SECTION("eular constructor")
    {
        REQUIRE_THAT(QuatF(0.0f, 0.0f, 0.0f), WithinAbs(QuatF(0.0f, 0.0f, 0.0f, 1.0f)));
        REQUIRE_THAT(QuatF(1.0f, -1.0f, 1.0f), WithinAbs(QuatF(0.0f, 0.0f, 0.0f, 1.0f)));
        REQUIRE_THAT(QuatF(-1.0f, 1.0f, -1.0f), WithinAbs(QuatF(0.0f, 0.0f, 0.0f, 1.0f)));

        REQUIRE_THAT(QuatF(0.25f, 0.0f, 0.0f), WithinAbs(QuatF(0.707107f, 0.0f, 0.0f, 0.707107f)));
        REQUIRE_THAT(QuatF(-0.5f, 0.0f, 0.0f), WithinAbs(QuatF(-1.0f, 0.0f, 0.0f, 0.0f)));
        REQUIRE_THAT(QuatF(0.75f, 0.0f, 0.0f), WithinAbs(QuatF(0.707107f, 0.0f, 0.0f, -0.707107f)));

        REQUIRE_THAT(QuatF(0.0f, 0.25f, 0.0f), WithinAbs(QuatF(0.0f, 0.707107f, 0.0f, 0.707107f)));
        REQUIRE_THAT(QuatF(0.0f, -0.5f, 0.0f), WithinAbs(QuatF(0.0f, -1.0f, 0.0f, 0.0f)));
        REQUIRE_THAT(QuatF(0.0f, 0.75f, 0.0f), WithinAbs(QuatF(0.0f, 0.707107f, 0.0f, -0.707107f)));

        REQUIRE_THAT(QuatF(0.0f, 0.0f, 0.25f), WithinAbs(QuatF(0.0f, 0.0f, 0.707107f, 0.707107f)));
        REQUIRE_THAT(QuatF(0.0f, 0.0f, -0.5f), WithinAbs(QuatF(0.0f, 0.0f, -1.0f, 0.0f)));
        REQUIRE_THAT(QuatF(0.0f, 0.0f, 0.75f), WithinAbs(QuatF(0.0f, 0.0f, 0.707107f, -0.707107f)));
    }

    SECTION("matrix constructor")
    {
        REQUIRE_THAT(QuatF(Mat3F()), WithinAbs(QuatF(0.0f, 0.0f, 0.0f, 1.0f)));
        REQUIRE_THAT(QuatF(Mat3F(QuatF(0.1f, 0.2f, 0.3f))), WithinAbs(QuatF(0.1f, 0.2f, 0.3f)));
    }
}

//============================================================================//

TEST_CASE("quaternion arithmetic operators tests", "[maths]")
{
    SECTION("quaternion / scalar binary operators")
    {
        REQUIRE_THAT(QuatF(1.0f, 2.0f, 3.0f, 4.0f) * 2.0f, WithinAbs(QuatF(2.0f, 4.0f, 6.0f, 8.0f)));
        REQUIRE_THAT(QuatF(1.0f, 2.0f, 3.0f, 4.0f) / 2.0f, WithinAbs(QuatF(0.5f, 1.0f, 1.5f, 2.0f)));
    }

    SECTION("quaternion / quaternion multiplication")
    {
        REQUIRE_THAT(QuatF() * QuatF(1.f, 0.f, 0.f, 0.f), WithinAbs(QuatF(1.f, 0.f, 0.f, 0.f)));
        REQUIRE_THAT(QuatF(0.f, 0.f, 1.f, 0.f) * QuatF(), WithinAbs(QuatF(0.f, 0.f, 1.f, 0.f)));
    }

    SECTION("quaternion / vector multiplication")
    {
        REQUIRE_THAT(QuatF(0.5f, 0.0f, 0.0f) * Vec3F(1.f, 2.f, 3.f), WithinAbs(Vec3F(1.f, -2.f, -3.f)));
        REQUIRE_THAT(QuatF(0.0f, 0.5f, 0.0f) * Vec3F(1.f, 2.f, 3.f), WithinAbs(Vec3F(-1.f, 2.f, -3.f)));
        REQUIRE_THAT(QuatF(0.0f, 0.0f, 0.5f) * Vec3F(1.f, 2.f, 3.f), WithinAbs(Vec3F(-1.f, -2.f, 3.f)));
    }
}

//============================================================================//

TEST_CASE("quaternion function tests", "[maths]")
{
    SECTION("quaternion slerp")
    {
        REQUIRE_THAT(maths::slerp(QuatF(), QuatF(0.f, 0.4f, 0.f), 0.5f), WithinAbs(QuatF(0.f, 0.2f, 0.f)));

        // will fallback to lerp
        const QuatF slerp180 = maths::slerp(QuatF(), QuatF(0.f, 0.5f, 0.f), 0.5f);
        REQUIRE_THAT(slerp180, WithinAbs(QuatF(0.f, 0.25f, 0.f)) || WithinAbs(QuatF(0.f, -0.25f, 0.f)));
    }
}
