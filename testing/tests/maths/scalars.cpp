#include "../../catch.hpp"
#include "../../Common.hpp"

//============================================================================//

TEST_CASE("scalar function tests", "[maths]")
{
    SECTION("variadic minimum")
    {
        REQUIRE(maths::min(3, 4) == 3);
        REQUIRE(maths::min(4, 3) == 3);
        REQUIRE(maths::min(2, 3, 4) == 2);
        REQUIRE(maths::min(4, 3, 2) == 2);
        REQUIRE(maths::min(1, 2, 3, 4) == 1);
        REQUIRE(maths::min(4, 3, 2, 1) == 1);
    }

    SECTION("variadic maximum")
    {
        REQUIRE(maths::max(1, 2) == 2);
        REQUIRE(maths::max(2, 1) == 2);
        REQUIRE(maths::max(1, 2, 3) == 3);
        REQUIRE(maths::max(3, 2, 1) == 3);
        REQUIRE(maths::max(1, 2, 3, 4) == 4);
        REQUIRE(maths::max(4, 3, 2, 1) == 4);
    }

    SECTION("clamp to range")
    {
        REQUIRE(maths::clamp(0, -5, +5) == 0);
        REQUIRE(maths::clamp(-9, -5, +5) == -5);
        REQUIRE(maths::clamp(+9, -5, +5) == +5);
    }

    SECTION("linear mix")
    {
        REQUIRE(maths::mix(-10.0f, +10.0f, 0.5f) == Approx(0.0f));
        REQUIRE(maths::mix(-10.0f, +10.0f, 0.25f) == Approx(-5.0f));
        REQUIRE(maths::mix(-10.0f, +10.0f, 0.75f) == Approx(+5.0f));
    }

    SECTION("convert to radians")
    {
        REQUIRE(maths::radians(0.0f) == Approx(0.0f));
        REQUIRE(maths::radians(0.5f) == Approx(PI));
    }

    SECTION("convert to cycles")
    {
        REQUIRE(maths::cycles(0.0f) == Approx(0.0f));
        REQUIRE(maths::cycles(PI) == Approx(0.5f));
    }
}
