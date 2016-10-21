#include <sqee/builtins.hpp>
#include <sqee/misc/Algorithms.hpp>

namespace sqt {

struct IdCounter
{
    int32_t next() { return ++previous; }
    int32_t previous = -1;
};

template<class Type>
struct DataEntry
{
    const int32_t id;
    Type& data;
};

template<class Type>
struct DataTable
{
    IdCounter counter;
    vector<int32_t> ids;
    vector<Type> data;
};

namespace dop {

template<class Type>
DataEntry<Type> insert(DataTable<Type>& table, Type data)
{
    table.ids.push_back(table.counter.next());
    table.data.push_back(std::move(data));

    return {table.ids.back(), table.data.back()};
}

template<class Type>
Type& get(DataTable<Type>& table, int32_t id)
{
    auto iter = sq::algo::find(table.ids, id);
    auto index = size_t(iter - table.ids.begin());
    return table.data[index];
}

}

}
