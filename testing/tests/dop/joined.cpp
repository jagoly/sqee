#include "../../catch.hpp"
#include "../../Common.hpp"

//============================================================================//

TEST_CASE("'joined' functions", "[dop]")
{
    dop::Group group;
    dop::Table<float> tableA;
    dop::Table<double> tableB;
    dop::Table<char> tableC;

    group.mIds = { 2, 4, 6, 7, 9, 11, 20 };
    tableA.mIds = { 1, 2, 6, 7, 9, 11, 20 };
    tableB.mIds = { 1, 2, 6, 7, 9, 20 };
    tableC.mIds = { 1, 4, 6, 7, 11, 20 };

    tableA.mData.resize(tableA.mIds.size(), 1.f);
    tableB.mData.resize(tableB.mIds.size(), 2.5);
    tableC.mData.resize(tableC.mIds.size(), 'a');

    SECTION("join one table")
    {
        auto a = dop::joined(tableA);
        auto gb = dop::joined(group, tableB);

        REQUIRE(a.size() == 7u);
        REQUIRE(gb.size() == 5u);
    }

    SECTION("join two tables")
    {
        auto ab = dop::joined(tableA, tableB);
        auto gcb = dop::joined(group, tableC, tableB);

        REQUIRE(ab.size() == 6u);
        REQUIRE(gcb.size() == 3u);
    }

    SECTION("join three tables")
    {
        auto abc = dop::joined(tableA, tableB, tableC);
        auto gcba = dop::joined(group, tableC, tableB, tableA);

        REQUIRE(abc.size() == 4u);
        REQUIRE(gcba.size() == 3u);
    }
}
