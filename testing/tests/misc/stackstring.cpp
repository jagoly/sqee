#include "../../catch.hpp"
#include "../../Common.hpp"

#include <sqee/misc/StackString.hpp>

//============================================================================//

constexpr const char CHAR_ARRAY[] = "abcdefghijklmno";

TEST_CASE("StackString", "[misc]")
{
    SECTION("comparison operators")
    {
        constexpr TinyString strJunkC = CHAR_ARRAY;

        static_assert (strJunkC == TinyString(CHAR_ARRAY), "");
        static_assert (strJunkC == TinyString("abcdefghijklmno"), "");
        static_assert (strJunkC < TinyString("bbcdefghijklmno"), "");
        static_assert (strJunkC > TinyString("abcdefghijklmn"), "");

        static_assert (strJunkC == CHAR_ARRAY, "");
        static_assert (strJunkC == "abcdefghijklmno", "");
        static_assert (strJunkC < "abcdefghijklmnoo", "");
        static_assert (strJunkC > "abcdefghijklmn", "");

        static_assert (StringView(CHAR_ARRAY) == strJunkC, "");
        static_assert (StringView("abcdefghijklmno") == strJunkC, "");
        static_assert (StringView("abcdefghijklmnoo") > strJunkC, "");
        static_assert (StringView("abcdefghijklmn") < strJunkC, "");

        TinyString strDefault;
        TinyString strEmpty = "";
        TinyString strJunk = "abcdefghijklmno";

        strJunk.clear();

        REQUIRE(strDefault == strEmpty);
        REQUIRE(strDefault == strJunk);
        REQUIRE(!(strDefault < strJunk));
        REQUIRE(!(strDefault > strJunk));

        REQUIRE(TinyString("Apple") == TinyString("Apple"));
        REQUIRE(TinyString("Apple") == "Apple");
        REQUIRE(TinyString("Apple") == StringView("Apple"));
        REQUIRE(TinyString("Apple") == String("Apple"));

        REQUIRE(TinyString("Apple") < TinyString("Apples"));
        REQUIRE(TinyString("Apple") < "Apples");
        REQUIRE(TinyString("Apple") < StringView("Apples"));
        REQUIRE(TinyString("Apple") < String("Apples"));
    }
}
