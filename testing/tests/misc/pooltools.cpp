#include "../../catch.hpp"
#include "../../Common.hpp"

#include <sqee/misc/PoolTools.hpp>

//============================================================================//

TEST_CASE("PoolTools", "[misc]")
{
    SECTION("PoolMap")
    {
        sq::PoolAllocatorStore<std::pair<const TinyString, std::string>> alloc(128u);
        sq::PoolMap<TinyString, std::string> map(alloc);

        map.reserve(2u);

        auto [one, ok_one] = map.try_emplace("one", "ONE");
        REQUIRE(ok_one == true);

        REQUIRE(map.size() == 1u);
        REQUIRE(map["one"] == "ONE");
        REQUIRE(&map["one"] == &map.find("one")->second);
        REQUIRE(&map["one"] == &one->second);
        REQUIRE(map.find("ONE") == map.end());

        auto [two, ok_two] = map.try_emplace("two", "TWO");
        REQUIRE(ok_two == true);

        REQUIRE(map.size() == 2u);
        REQUIRE(map["one"] == "ONE");
        REQUIRE(map["two"] == "TWO");
        REQUIRE(&map["one"] == &map.find("one")->second);
        REQUIRE(&map["two"] == &map.find("two")->second);
        REQUIRE(&map["one"] == &one->second);
        REQUIRE(&map["two"] == &two->second);
        REQUIRE(map.find("TWO") == map.end());

        std::string* one_ptr = &one->second;
        std::string* two_ptr = &two->second;

        auto [three, ok_three] = map.try_emplace("three", "THREE");
        REQUIRE(ok_three == true);

        std::string& oneB = map["one"];
        std::string& twoB = map["two"];

        REQUIRE(one_ptr == &oneB);
        REQUIRE(two_ptr == &twoB);

        REQUIRE(map.size() == 3u);
        REQUIRE(&map["one"] == one_ptr);
        REQUIRE(&map["two"] == two_ptr);
        REQUIRE(&map["three"] == &three->second);
        REQUIRE(map["one"] == "ONE");
        REQUIRE(map["two"] == "TWO");
        REQUIRE(map["three"] == "THREE");

        std::string* three_ptr = &three->second;

        map.erase(map.find("two"));

        std::string& oneC = map["one"];
        std::string& threeC = map["three"];

        REQUIRE(one_ptr == &oneC);
        REQUIRE(three_ptr == &threeC);

        REQUIRE(map.size() == 2u);
        REQUIRE(map.find("two") == map.end());
        REQUIRE(map["one"] == "ONE");
        REQUIRE(map["three"] == "THREE");
    }
}
