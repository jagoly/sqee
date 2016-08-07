#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <sqee/misc/StringCast.hpp>
#include <sqee/maths/Volumes.hpp>


namespace sq {
std::ostream& operator<<(std::ostream& _os, const Vec2F& _value) { _os << sq::chai_string(_value); return _os; }
std::ostream& operator<<(std::ostream& _os, const Vec3F& _value) { _os << sq::chai_string(_value); return _os; }
std::ostream& operator<<(std::ostream& _os, const Vec4F& _value) { _os << sq::chai_string(_value); return _os; }
}


TEST_CASE("volume intersection tests", "[volumes]") {

    SECTION("unit BoundBox with no transform") {
        auto bbox = sq::make_BoundBox(Mat4F(), {0.f, 0.f, 0.f}, 1.0f, {1.f, 1.f, 1.f});

        REQUIRE(bbox.radius == 1.f);
        REQUIRE(bbox.origin == Vec3F(0.f, 0.0f, 0.f));
        REQUIRE(bbox.points[0] == Vec3F(-1.f, -1.f, -1.f));
        REQUIRE(bbox.points[1] == Vec3F(-1.f, -1.f, +1.f));
        REQUIRE(bbox.points[2] == Vec3F(-1.f, +1.f, -1.f));
        REQUIRE(bbox.points[3] == Vec3F(-1.f, +1.f, +1.f));
        REQUIRE(bbox.points[4] == Vec3F(+1.f, -1.f, -1.f));
        REQUIRE(bbox.points[5] == Vec3F(+1.f, -1.f, +1.f));
        REQUIRE(bbox.points[6] == Vec3F(+1.f, +1.f, -1.f));
        REQUIRE(bbox.points[7] == Vec3F(+1.f, +1.f, +1.f));
    }
}
