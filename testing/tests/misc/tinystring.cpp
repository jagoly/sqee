#include "../../catch.hpp"
#include "../../Common.hpp"

//============================================================================//

constexpr const char str[] = "abcdefghijklmno";

TEST_CASE("TinyString", "[misc]")
{
    SECTION("comparison operators")
    {
        TinyString strDefault;
        TinyString strEmpty = "";
        TinyString strJunk = "abcdefghijklmno";
        constexpr TinyString strJfnk = str;

        static_assert (strJfnk == str, "");
        static_assert (strJfnk == "abcdefghijklmno", "");
        strJunk.clear();

        REQUIRE(strDefault == strEmpty);
        REQUIRE(strDefault == strJunk);
        REQUIRE(!(strDefault < strJunk));
        REQUIRE(!(strDefault > strJunk));

        //REQUIRE(TinyString("Apple") == TinyString("Apple"));
        REQUIRE(TinyString("Apples") != TinyString("Apple"));
        //REQUIRE(TinyString("Apples") > TinyString("Apple"));
        //REQUIRE(TinyString("Apple") < TinyString("Apples"));
        //REQUIRE(TinyString("Bpple") != TinyString("Apple"));
        //REQUIRE(TinyString("Bpple") > TinyString("Apple"));
    }
}
