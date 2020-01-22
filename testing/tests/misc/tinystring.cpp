#include "../../catch.hpp"
#include "../../Common.hpp"

//============================================================================//

TEST_CASE("TinyString", "[misc]")
{
    SECTION("comparison operators")
    {
        TinyString strDefault;
        TinyString strEmpty = "";
        TinyString strJunk = "abcdefghijklmno";
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
