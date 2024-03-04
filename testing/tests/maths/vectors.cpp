#include <sqee/maths/Vectors.hpp>

#include "Common.hpp"

//============================================================================//

TEST_CASE("vector equality and constructor tests", "[maths]")
{
    SECTION("equality / inequality")
    {
        constexpr auto v2 = Vec2F(1.f, 2.f);
        constexpr auto v3 = Vec3U(1u, 2u, 3u);
        constexpr auto v4 = Vec4I(1, 2, 3, 4);

        REQUIRE(v2 == Vec2F(1.f, 2.f));
        REQUIRE(v2 != Vec2F(1.f, 0.f));
        REQUIRE(v2 != Vec2F(0.f, 2.f));
        REQUIRE(v2 != Vec2F(0.f, 0.f));

        REQUIRE(v3 == Vec3U(1u, 2u, 3u));
        REQUIRE(v3 != Vec3U(1u, 0u, 0u));
        REQUIRE(v3 != Vec3U(0u, 0u, 3u));
        REQUIRE(v3 != Vec3U(0u, 0u, 0u));

        REQUIRE(v4 == Vec4I(1, 2, 3, 4));
        REQUIRE(v4 != Vec4I(1, 0, 0, 0));
        REQUIRE(v4 != Vec4I(0, 0, 0, 4));
        REQUIRE(v4 != Vec4I(0, 0, 0, 0));
    }

    SECTION("scalar constructors")
    {
        REQUIRE(Vec2F(2.f) == Vec2F(2.f, 2.f));
        REQUIRE(Vec3U(3u) == Vec3U(3u, 3u, 3u));
        REQUIRE(Vec4I(4) == Vec4I(4, 4, 4, 4));
    }

    SECTION("swizzle constructors")
    {
        constexpr auto v2 = Vec2I(1, 2);
        constexpr auto v3 = Vec3I(3, 4, 5);
        constexpr auto v4 = Vec4I(6, 7, 8, 9);

        REQUIRE(Vec2I(v3) == Vec2I(3, 4));
        REQUIRE(Vec2I(v4) == Vec2I(6, 7));

        REQUIRE(Vec3I(v4) == Vec3I(6, 7, 8));
        REQUIRE(Vec3I(v2, 10) == Vec3I(1, 2, 10));
        REQUIRE(Vec3I(10, v2) == Vec3I(10, 1, 2));

        REQUIRE(Vec4I(v2, 10, 20) == Vec4I(1, 2, 10, 20));
        REQUIRE(Vec4I(10, v2, 20) == Vec4I(10, 1, 2, 20));
        REQUIRE(Vec4I(10, 20, v2) == Vec4I(10, 20, 1, 2));
        REQUIRE(Vec4I(v3, 10) == Vec4I(3, 4, 5, 10));
        REQUIRE(Vec4I(10, v3) == Vec4I(10, 3, 4, 5));
    }
}

//============================================================================//

TEST_CASE("vector arithmetic operators tests", "[maths]")
{
    SECTION("vector / vector binary operators")
    {
        REQUIRE(Vec2I(0, 1)       + Vec2I(1, 2)       == Vec2I(1, 3));
        REQUIRE(Vec3I(0, 1, 2)    + Vec3I(2, 3, 4)    == Vec3I(2, 4, 6));
        REQUIRE(Vec4I(0, 1, 2, 3) + Vec4I(3, 4, 5, 6) == Vec4I(3, 5, 7, 9));

        REQUIRE(Vec2I(0, 1)       - Vec2I(2, 1)       == Vec2I(-2, 0));
        REQUIRE(Vec3I(0, 1, 2)    - Vec3I(4, 3, 2)    == Vec3I(-4, -2, 0));
        REQUIRE(Vec4I(0, 1, 2, 3) - Vec4I(6, 5, 4, 3) == Vec4I(-6, -4, -2, 0));

        REQUIRE(Vec2I(4, 3)       * Vec2I(2, 1)       == Vec2I(8, 3));
        REQUIRE(Vec3I(6, 5, 4)    * Vec3I(3, 2, 1)    == Vec3I(18, 10, 4));
        REQUIRE(Vec4I(8, 7, 6, 5) * Vec4I(4, 3, 2, 1) == Vec4I(32, 21, 12, 5));

        REQUIRE(Vec2I(12, 6)         / Vec2I(3, 2)       == Vec2I(4, 3));
        REQUIRE(Vec3I(20, 12, 6)     / Vec3I(4, 3, 2)    == Vec3I(5, 4, 3));
        REQUIRE(Vec4I(30, 20, 12, 6) / Vec4I(5, 4, 3, 2) == Vec4I(6, 5, 4, 3));
    }

    SECTION("vector / scalar binary operators")
    {
        REQUIRE(Vec2I(1, 2)       + 3 == Vec2I(4, 5));
        REQUIRE(Vec3I(1, 2, 3)    + 4 == Vec3I(5, 6, 7));
        REQUIRE(Vec4I(1, 2, 3, 4) + 5 == Vec4I(6, 7, 8, 9));

        REQUIRE(Vec2I(1, 2)       - 3 == Vec2I(-2, -1));
        REQUIRE(Vec3I(1, 2, 3)    - 4 == Vec3I(-3, -2, -1));
        REQUIRE(Vec4I(1, 2, 3, 4) - 5 == Vec4I(-4, -3, -2, -1));

        REQUIRE(Vec2I(1, 2)       * 2 == Vec2I(2, 4));
        REQUIRE(Vec3I(1, 2, 3)    * 3 == Vec3I(3, 6, 9));
        REQUIRE(Vec4I(1, 2, 3, 4) * 4 == Vec4I(4, 8, 12, 16));

        REQUIRE(Vec2I(4, 6)          / 2 == Vec2I(2, 3));
        REQUIRE(Vec3I(6, 9, 12)      / 3 == Vec3I(2, 3, 4));
        REQUIRE(Vec4I(8, 12, 16, 20) / 4 == Vec4I(2, 3, 4, 5));
    }

    SECTION("vector unary operators")
    {
        REQUIRE(-Vec2I(1, 2)       == +Vec2I(-1, -2));
        REQUIRE(-Vec3I(1, 2, 3)    == +Vec3I(-1, -2, -3));
        REQUIRE(-Vec4I(1, 2, 3, 4) == +Vec4I(-1, -2, -3, -4));
    }
}

//============================================================================//

TEST_CASE("vector function tests", "[maths]")
{
    SECTION("minimum / maximum")
    {
        REQUIRE(maths::min(Vec2I(1, 3), Vec2I(4, 2)) == Vec2I(1, 2));
        REQUIRE(maths::max(Vec2I(1, 3), Vec2I(4, 2)) == Vec2I(4, 3));

        REQUIRE(maths::min(Vec3I(1, 3, 5), Vec3I(6, 4, 2)) == Vec3I(1, 3, 2));
        REQUIRE(maths::max(Vec3I(1, 3, 5), Vec3I(6, 4, 2)) == Vec3I(6, 4, 5));

        REQUIRE(maths::min(Vec4I(1, 3, 5, 7), Vec4I(8, 6, 4, 2)) == Vec4I(1, 3, 4, 2));
        REQUIRE(maths::max(Vec4I(1, 3, 5, 7), Vec4I(8, 6, 4, 2)) == Vec4I(8, 6, 5, 7));
    }

    SECTION("clamp to range")
    {
        constexpr auto lower = Vec2I(-10, -20);
        constexpr auto upper = Vec2I(+10, +20);

        REQUIRE(maths::clamp(Vec2I(-5, +10), lower, upper) == Vec2I(-5, +10));
        REQUIRE(maths::clamp(Vec2I(+5, -10), lower, upper) == Vec2I(+5, -10));
        REQUIRE(maths::clamp(Vec2I(-15, +30), lower, upper) == Vec2I(-10, +20));
        REQUIRE(maths::clamp(Vec2I(+15, -30), lower, upper) == Vec2I(+10, -20));

        REQUIRE(maths::clamp(lower, lower, upper) == lower);
        REQUIRE(maths::clamp(upper, lower, upper) == upper);
    }

    SECTION("linear mix")
    {
        constexpr auto lower = Vec2F(-10, -20);
        constexpr auto upper = Vec2F(+10, +20);

        REQUIRE_THAT(maths::mix(lower, upper, 0.5f), WithinAbs(Vec2F(0, 0)));
        REQUIRE_THAT(maths::mix(lower, upper, 0.25f), WithinAbs(Vec2F(-5, -10)));
        REQUIRE_THAT(maths::mix(lower, upper, 0.75f), WithinAbs(Vec2F(+5, +10)));

        REQUIRE_THAT(maths::mix(lower, upper, 0.0f), WithinAbs(lower));
        REQUIRE_THAT(maths::mix(lower, upper, 1.0f), WithinAbs(upper));
    }

    SECTION("dot product")
    {
        REQUIRE_THAT(maths::dot(Vec2F(3, 2), Vec2F(5, 4)), WithinAbs(23.0f));
        REQUIRE_THAT(maths::dot(Vec3F(4, 3, 2), Vec3F(7, 6, 5)), WithinAbs(56.0f));
        REQUIRE_THAT(maths::dot(Vec4F(5, 4, 3, 2), Vec4F(9, 8, 7, 6)), WithinAbs(110.0f));
    }

    SECTION("length")
    {
        REQUIRE_THAT(maths::length(Vec2F(1.0f, 0.0f)), WithinAbs(1.0f));
        REQUIRE_THAT(maths::length(Vec2F(0.0f, 1.0f)), WithinAbs(1.0f));
        REQUIRE_THAT(maths::length(Vec2F(1.0f, 1.0f)), WithinAbs(1.414214f));

        REQUIRE_THAT(maths::length(Vec3F(0.5f, 0.0f, 0.0f)), WithinAbs(0.5f));
        REQUIRE_THAT(maths::length(Vec3F(0.5f, 0.5f, 0.0f)), WithinAbs(0.707107f));
        REQUIRE_THAT(maths::length(Vec3F(0.5f, 0.5f, 0.5f)), WithinAbs(0.866025f));

        REQUIRE_THAT(maths::length(Vec4F(0.2f, 0.4f, 0.6f, 0.8f)), WithinAbs(1.095445f));
        REQUIRE_THAT(maths::length(Vec4F(4.4f, 3.3f, 2.2f, 1.1f)), WithinAbs(6.024948f));
    }

    SECTION("normalize")
    {
        REQUIRE_THAT(maths::normalize(Vec2F(0.5, 0.0)), WithinAbs(Vec2F(1.0, 0.0)));
        REQUIRE_THAT(maths::normalize(Vec3F(0.0, 1.0, 0.0)), WithinAbs(Vec3F(0.0, 1.0, 0.0)));
        REQUIRE_THAT(maths::normalize(Vec4F(0.0, 0.0, 1.5, 0.0)), WithinAbs(Vec4F(0.0, 0.0, 1.0, 0.0)));

        REQUIRE_THAT(maths::normalize(Vec2F(3.3f, 5.5f)), WithinAbs(Vec2F(0.514496f, 0.857493f)));
    }

    SECTION("reflect")
    {
        const auto pNormA = Vec3F(-1.0f, 0.0f, 0.0f);
        const auto pNormB = Vec3F(0.0f, 0.707107f, 0.707107f);

        REQUIRE_THAT(maths::reflect(Vec3F(1.0f, 0.0f, 0.0f), pNormA), WithinAbs(Vec3F(-1.0f, 0.0f, 0.0f)));
        REQUIRE_THAT(maths::reflect(Vec3F(1.0f, 0.0f, 1.0f), pNormA), WithinAbs(Vec3F(-1.0f, 0.0f, 1.0f)));

        REQUIRE_THAT(maths::reflect(Vec3F(0.0f, 0.0f, -1.0f), pNormB), WithinAbs(Vec3F(0.0f, 1.0f, 0.0f)));
        REQUIRE_THAT(maths::reflect(Vec3F(0.0f, -1.0f, 0.0f), pNormB), WithinAbs(Vec3F(0.0f, 0.0f, 1.0f)));
    }

    SECTION("cross product")
    {
        const auto normal = Vec3F(1.0f, 0.0f, 0.0f);
        const auto tangent = Vec3F(0.0f, 1.0f, 0.0f);
        const auto bitangent = Vec3F(0.0f, 0.0f, 1.0f);

        REQUIRE_THAT(maths::normalize(maths::cross(normal, tangent)), WithinAbs(bitangent));
        REQUIRE_THAT(maths::normalize(maths::cross(tangent, bitangent)), WithinAbs(normal));
        REQUIRE_THAT(maths::normalize(maths::cross(bitangent, normal)), WithinAbs(tangent));

        REQUIRE_THAT(maths::normalize(maths::cross(tangent, normal)), WithinAbs(-bitangent));
        REQUIRE_THAT(maths::normalize(maths::cross(bitangent, tangent)), WithinAbs(-normal));
        REQUIRE_THAT(maths::normalize(maths::cross(normal, bitangent)), WithinAbs(-tangent));
    }

    SECTION("rotate along axis")
    {
        const auto vec = Vec3F(1.0, 2.0, 3.0);

        REQUIRE_THAT(maths::rotate_x(vec, 0.25f), WithinAbs(Vec3F(1.0, -3.0, 2.0)));
        REQUIRE_THAT(maths::rotate_x(vec, -0.5f), WithinAbs(Vec3F(1.0, -2.0, -3.0)));
        REQUIRE_THAT(maths::rotate_x(vec, 0.75f), WithinAbs(Vec3F(1.0, 3.0, -2.0)));

        REQUIRE_THAT(maths::rotate_y(vec, 0.25f), WithinAbs(Vec3F(3.0, 2.0, -1.0)));
        REQUIRE_THAT(maths::rotate_y(vec, -0.5f), WithinAbs(Vec3F(-1.0, 2.0, -3.0)));
        REQUIRE_THAT(maths::rotate_y(vec, 0.75f), WithinAbs(Vec3F(-3.0, 2.0, 1.0)));

        REQUIRE_THAT(maths::rotate_z(vec, 0.25f), WithinAbs(Vec3F(-2.0, 1.0, 3.0)));
        REQUIRE_THAT(maths::rotate_z(vec, -0.5f), WithinAbs(Vec3F(-1.0, -2.0, 3.0)));
        REQUIRE_THAT(maths::rotate_z(vec, 0.75f), WithinAbs(Vec3F(2.0, -1.0, 3.0)));
    }
}
