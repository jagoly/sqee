#include <sqee/misc/Json.hpp>

#include "Common.hpp"

//============================================================================//

TEST_CASE("Json", "[misc]")
{
    constexpr std::array<std::pair<StringView, uint64_t>, 4> stringIntPairs = {{
        {"zero", 0}, {"one", 1}, {"two", 2}, {"three", 3}
    }};

    constexpr const char srcArrayOfStrings[] = R"([ "zero", "one", "two", "three" ])";

    SECTION("array of strings")
    {
        const auto document = JsonDocument::parse_string(srcArrayOfStrings, "array_of_strings");
        const auto json = document.root().as<JsonArray>();

        REQUIRE(json[0].as<StringView>() == "zero");
        REQUIRE(json[1].as<StringView>() == "one");
        REQUIRE(json[2].as<String>() == "two");
        REQUIRE(json[3].as<TinyString>() == "three");

        REQUIRE_THROWS_AS(json[4], std::runtime_error);

        auto iter = stringIntPairs.begin();
        for (const auto [index, value] : json)
        {
            REQUIRE(index == iter->second);
            REQUIRE(value.as<StringView>() == iter->first);
            ++iter;
        }
        REQUIRE(iter == stringIntPairs.end());
    }

    constexpr const char srcObjectOfInts[] = R"({ "zero": 0, "one": 1, "two": 2, "three": 3 })";

    SECTION("object of ints")
    {
        const auto document = JsonDocument::parse_string(srcObjectOfInts, "object_of_ints");
        const auto json = document.root().as<JsonObject>();

        REQUIRE(json["zero"].as<int64_t>() == 0);
        REQUIRE(json["one"].as<int8_t>() == 1);
        REQUIRE(json["two"].as<uint64_t>() == 2);
        REQUIRE(json["three"].as<uint8_t>() == 3);

        REQUIRE_THROWS_AS(json["four"], std::runtime_error);
        REQUIRE_THROWS_AS(json["zero"].as<double>(), std::runtime_error);

        auto iter = stringIntPairs.begin();
        for (const auto [key, value] : json)
        {
            REQUIRE(key == iter->first);
            REQUIRE(value.as<uint64_t>() == iter->second);
            ++iter;
        }
        REQUIRE(iter == stringIntPairs.end());
    }

    constexpr const char srcMixedValues[] = R"({ "nullptr": null, "bool": false, "double": 69.0 })";

    SECTION("mixed values")
    {
        const auto document = JsonDocument::parse_string(srcMixedValues, "mixed_values");
        const auto json = document.root().as<JsonObject>();

        REQUIRE(json["nullptr"].as<std::nullptr_t>() == nullptr);
        REQUIRE(json["bool"].as<bool>() == false);
        REQUIRE(json["double"].as<double>() == 69.0);
    }
}
