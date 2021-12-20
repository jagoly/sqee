#include "../../catch.hpp"
#include "../../Common.hpp"

#include <sqee/misc/StackVector.hpp>

//============================================================================//

TEST_CASE("StackVector", "[misc]")
{
    using TestType = sq::StackVector<String, 4>;

    SECTION("initializer_list construction")
    {
        TestType animals = { "chicken", "dog", "penguin", "horse" };

        REQUIRE(animals.size() == 4);
        REQUIRE(animals.full() == true);
        REQUIRE(animals.front() == "chicken");
        REQUIRE(animals.back() == "horse");
    }

    SECTION("emplace_back construction")
    {
        TestType animals;
        animals.emplace_back("chicken");
        animals.emplace_back(String("dog"));
        animals.emplace_back() = "penguin";
        animals.emplace_back() = String("horse");

        REQUIRE(animals.size() == 4);
        REQUIRE(animals.full() == true);
        REQUIRE(animals.front() == "chicken");
        REQUIRE(animals.back() == "horse");
    }

    SECTION("other")
    {
        TestType animals = { "chicken", "dog", "penguin", "horse" };
        TestType copy = animals;

        REQUIRE(animals == copy);

        copy[2] = "orca";
        REQUIRE(animals != copy);

        copy = animals;
        REQUIRE(copy == animals);

        animals.erase(animals.begin() + 1);
        REQUIRE(animals != copy);
        REQUIRE(animals.size() == 3);
        REQUIRE(animals[2] == "horse");
    }
}
