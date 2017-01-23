#pragma once

#include <tuple>

#include <sqee/dop/Classes.hpp>
#include <sqee/maths/Scalar.hpp>

namespace sq { namespace dop {

//============================================================================//

template <class In, class Out>
using transfer_const_type = std::conditional_t<std::is_const<In>::value, const Out, Out>;

template <class Table>
using table_data_type = transfer_const_type<Table, typename Table::data_type>;

template <class... Tables>
using join_entry_type = std::tuple<int32_t, table_data_type<Tables>&...>;

template <class... Tables>
using join_vector_type = std::vector<join_entry_type<Tables...>>;

template <class Table>
using entry_type = Entry<table_data_type<Table>>;

template <class Table>
using entry_vector_type = std::vector<entry_type<Table>>;

//============================================================================//

void impl_insertion_sort(std::vector<int32_t>& ids);

//============================================================================//

inline void sort(const Group& group)
{
    // skip if group already sorted
    if (group.mIsSorted == true) return;

    // remove const and do the sort
    auto& mutGroup = const_cast<Group&>(group);
    impl_insertion_sort(mutGroup.mIds);

    mutGroup.mIsSorted = true;
}

//============================================================================//

template <class Type>
inline void sort(const Table<Type>& table)
{
    // skip if table already sorted
    if (table.mIsSorted == true) return;

    // remove const so that we can sort any table
    auto& mutTable = const_cast<Table<Type>&>(table);

    // create a sorted copy of the id vector
    std::vector<int32_t> sortedIds = table.mIds;
    impl_insertion_sort(sortedIds);

    // create, reserve and fill vector of sorted data
    std::vector<Type> sortedData; sortedData.reserve(sortedIds.size());
    for (auto& id : sortedIds) sortedData.push_back(std::move(mutTable.get(id)));

    mutTable.mIds = std::move(sortedIds);
    mutTable.mData = std::move(sortedData);

    mutTable.mIsSorted = true;
}

//============================================================================//

Group reduce(const Group& a, const Group& b);
Group reduce(const Group& a, const Group& b, const Group& c);
Group reduce(const Group& a, const Group& b, const Group& c, const Group& d);

//============================================================================//

// TODO: remove in C++17 in favour of joined + structured bindings

template <class Table> inline
entry_vector_type<Table> entries(Table& table)
{
    entry_vector_type<Table> result;
    result.reserve(table.size());

    for (uint i = 0u; i < table.size(); ++i)
        result.emplace_back(table.mIds[i], &table.mData[i]);

    return result;
}

//============================================================================//

template <class Table> inline
join_vector_type<Table> joined(Table& table)
{
    join_vector_type<Table> result;
    result.reserve(table.size());

    //========================================================//

    if (table.empty()) return result;

    dop::sort(table);

    //========================================================//

    for (uint i = 0u; i < table.size(); ++i)
    {
        const auto tupleId = std::make_tuple(table.mIds[i]);
        auto tupleA = std::tie(table.mData[i]);
        result.push_back(std::tuple_cat(tupleId, tupleA));
    }

    //========================================================//

    return result;
}

//============================================================================//

template <class TableA> inline
join_vector_type<TableA> joined(const Group& g, TableA& a)
{
    join_vector_type<TableA> result;
    const auto reserve = maths::min(g.size(), a.size());
    result.reserve(reserve);

    //========================================================//

    if (reserve == 0u) return result;

    dop::sort(g); dop::sort(a);

    //========================================================//

    for ( auto iterG = g.mIds.cbegin(), iterA = a.mIds.cbegin();; )
    {
        if (*iterG == *iterA)
        {
            const auto tupleId = std::make_tuple(*iterG);
            auto tupleA = std::tie(a.mData[std::distance(a.mIds.cbegin(), iterA)]);
            result.push_back(std::tuple_cat(tupleId, tupleA));

            if (++iterG == g.mIds.end()) break;
            if (++iterA == a.mIds.end()) break;
        }

        if ( *iterG < *iterA && ++iterG == g.mIds.cend() ) break;

        if ( *iterA < *iterG && ++iterA == a.mIds.cend() ) break;
    }

    //========================================================//

    result.shrink_to_fit();

    return result;
}

//============================================================================//

template <class TableA, class TableB> inline
join_vector_type<TableA, TableB> joined(TableA& a, TableB& b)
{
    join_vector_type<TableA, TableB> result;
    const auto reserve = maths::min(a.size(), b.size());
    result.reserve(reserve);

    //========================================================//

    if (reserve == 0u) return result;

    dop::sort(a); dop::sort(b);

    //========================================================//

    for ( auto iterA = a.mIds.cbegin(), iterB = b.mIds.cbegin();; )
    {
        if (*iterA == *iterB)
        {
            const auto tupleId = std::make_tuple(*iterA);
            auto tupleA = std::tie(a.mData[std::distance(a.mIds.cbegin(), iterA)]);
            auto tupleB = std::tie(b.mData[std::distance(b.mIds.cbegin(), iterB)]);
            result.push_back(std::tuple_cat(tupleId, tupleA, tupleB));

            if (++iterA == a.mIds.cend()) break;
            if (++iterB == b.mIds.cend()) break;
        }

        if ( *iterA < *iterB && ++iterA == a.mIds.cend() ) break;

        if ( *iterB < *iterA && ++iterB == b.mIds.cend() ) break;
    }

    //========================================================//

    result.shrink_to_fit();

    return result;
}

//============================================================================//

template <class TableA, class TableB> inline
join_vector_type<TableA, TableB> joined(const Group& g, TableA& a, TableB& b)
{
    join_vector_type<TableA, TableB> result;
    const auto reserve = maths::min(g.size(), a.size(), b.size());
    result.reserve(reserve);

    //========================================================//

    if (reserve == 0u) return result;

    dop::sort(g); dop::sort(a); dop::sort(b);

    //========================================================//

    for ( auto iterG = g.mIds.cbegin(), iterA = a.mIds.cbegin(), iterB = b.mIds.cbegin();; )
    {
        if (*iterG == *iterA && *iterG == *iterB)
        {
            const auto tupleId = std::make_tuple(*iterG);
            auto tupleA = std::tie(a.mData[std::distance(a.mIds.cbegin(), iterA)]);
            auto tupleB = std::tie(b.mData[std::distance(b.mIds.cbegin(), iterB)]);
            result.push_back(std::tuple_cat(tupleId, tupleA, tupleB));

            if (++iterG == g.mIds.end()) break;
            if (++iterA == a.mIds.end()) break;
            if (++iterB == b.mIds.end()) break;
        }

        if ( *iterG < *iterA && ++iterG == g.mIds.cend() ) break;
        if ( *iterG < *iterB && ++iterG == g.mIds.cend() ) break;

        if ( *iterA < *iterB && ++iterA == a.mIds.cend() ) break;
        if ( *iterA < *iterG && ++iterA == a.mIds.cend() ) break;

        if ( *iterB < *iterG && ++iterB == b.mIds.cend() ) break;
        if ( *iterB < *iterA && ++iterB == b.mIds.cend() ) break;
    }

    //========================================================//

    result.shrink_to_fit();

    return result;
}

//============================================================================//

template <class TableA, class TableB, class TableC> inline
join_vector_type<TableA, TableB, TableC> joined(TableA& a, TableB& b, TableC& c)
{
    join_vector_type<TableA, TableB, TableC> result;
    const auto reserve = maths::min(a.size(), b.size(), c.size());
    result.reserve(reserve);

    //========================================================//

    if (reserve == 0u) return result;

    dop::sort(a); dop::sort(b); dop::sort(c);

    //========================================================//

    for ( auto iterA = a.mIds.cbegin(), iterB = b.mIds.cbegin(), iterC = c.mIds.cbegin();; )
    {
        if (*iterA == *iterB && *iterA == *iterC)
        {
            const auto tupleId = std::make_tuple(*iterA);
            auto tupleA = std::tie(a.mData[std::distance(a.mIds.cbegin(), iterA)]);
            auto tupleB = std::tie(b.mData[std::distance(b.mIds.cbegin(), iterB)]);
            auto tupleC = std::tie(c.mData[std::distance(c.mIds.cbegin(), iterC)]);
            result.push_back(std::tuple_cat(tupleId, tupleA, tupleB, tupleC));

            if (++iterA == a.mIds.cend()) break;
            if (++iterB == b.mIds.cend()) break;
            if (++iterC == c.mIds.cend()) break;
        }

        if ( *iterA < *iterB && ++iterA == a.mIds.cend() ) break;
        if ( *iterA < *iterC && ++iterA == a.mIds.cend() ) break;

        if ( *iterB < *iterC && ++iterB == b.mIds.cend() ) break;
        if ( *iterB < *iterA && ++iterB == b.mIds.cend() ) break;

        if ( *iterC < *iterA && ++iterC == c.mIds.cend() ) break;
        if ( *iterC < *iterB && ++iterC == c.mIds.cend() ) break;
    }

    //========================================================//

    result.shrink_to_fit();

    return result;
}

//============================================================================//

template <class TableA, class TableB, class TableC> inline
join_vector_type<TableA, TableB, TableC> joined(const Group& g, TableA& a, TableB& b, TableC& c)
{
    join_vector_type<TableA, TableB, TableC> result;
    const auto reserve = maths::min(g.size(), a.size(), b.size(), c.size());
    result.reserve(reserve);

    //========================================================//

    if (reserve == 0u) return result;

    dop::sort(g); dop::sort(a); dop::sort(b); dop::sort(c);

    //========================================================//

    for ( auto iterG = g.mIds.cbegin(), iterA = a.mIds.cbegin(), iterB = b.mIds.cbegin(), iterC = c.mIds.cbegin();; )
    {
        if (*iterG == *iterA && *iterG == *iterB && *iterG == *iterC)
        {
            const auto tupleId = std::make_tuple(*iterG);
            auto tupleA = std::tie(a.mData[std::distance(a.mIds.cbegin(), iterA)]);
            auto tupleB = std::tie(b.mData[std::distance(b.mIds.cbegin(), iterB)]);
            auto tupleC = std::tie(c.mData[std::distance(c.mIds.cbegin(), iterC)]);
            result.push_back(std::tuple_cat(tupleId, tupleA, tupleB, tupleC));

            if (++iterG == g.mIds.cend()) break;
            if (++iterA == a.mIds.cend()) break;
            if (++iterB == b.mIds.cend()) break;
            if (++iterC == c.mIds.cend()) break;
        }

        if ( *iterG < *iterA && ++iterG == g.mIds.cend() ) break;
        if ( *iterG < *iterB && ++iterG == g.mIds.cend() ) break;
        if ( *iterG < *iterC && ++iterG == g.mIds.cend() ) break;

        if ( *iterA < *iterB && ++iterA == a.mIds.cend() ) break;
        if ( *iterA < *iterC && ++iterA == a.mIds.cend() ) break;
        if ( *iterA < *iterG && ++iterA == a.mIds.cend() ) break;

        if ( *iterB < *iterC && ++iterB == b.mIds.cend() ) break;
        if ( *iterB < *iterG && ++iterB == b.mIds.cend() ) break;
        if ( *iterB < *iterA && ++iterB == b.mIds.cend() ) break;

        if ( *iterC < *iterG && ++iterC == c.mIds.cend() ) break;
        if ( *iterC < *iterA && ++iterC == c.mIds.cend() ) break;
        if ( *iterC < *iterB && ++iterC == c.mIds.cend() ) break;
    }

    //========================================================//

    result.shrink_to_fit();

    return result;
}

//============================================================================//

}} // namespace sq::dop
