#include <algorithm>

#include <sqee/dop/Functions.hpp>

namespace dop = sq::dop;
namespace maths = sq::maths;
namespace algo = sq::algo;

//============================================================================//

void dop::impl_insertion_sort(std::vector<int32_t>& ids)
{
    for (auto iter = ids.begin(); iter != ids.end(); ++iter)
    {
         auto insertPoint = std::upper_bound(ids.begin(), iter, *iter);
         std::rotate(insertPoint, iter, std::next(iter));
    }
}

//============================================================================//

dop::Group dop::reduce(const dop::Group& a, const dop::Group& b)
{
    const auto reserve = maths::min(a.size(), b.size());
    dop::Group result; result.reserve(reserve);

    if (reserve == 0u) return result;

    dop::sort(a); dop::sort(b);

    //--------------------------------------------------------//

    for ( auto iterA = a.mIds.begin(), iterB = b.mIds.begin();; )
    {
        if (*iterA == *iterB)
        {
            result.mIds.push_back(*iterA);

            if (++iterA == a.mIds.end()) break;
            if (++iterB == b.mIds.end()) break;
        }

         if (*iterA < *iterB && ++iterA == a.mIds.end()) break;

         if (*iterB < *iterA && ++iterB == b.mIds.end()) break;
    };

    //--------------------------------------------------------//

    result.shrink_to_fit();

    return result;
}

//============================================================================//

dop::Group dop::reduce(const dop::Group& a, const dop::Group& b, const dop::Group& c)
{
    const auto reserve = maths::min(a.size(), b.size(), c.size());
    dop::Group result; result.reserve(reserve);

    if (reserve == 0u) return result;

    dop::sort(a); dop::sort(b); dop::sort(c);

    //--------------------------------------------------------//

    for ( auto iterA = a.mIds.begin(), iterB = b.mIds.begin(), iterC = c.mIds.begin();; )
    {
        if (*iterA == *iterB && *iterA == *iterC)
        {
            result.mIds.push_back(*iterA);

            if (++iterA == a.mIds.end()) break;
            if (++iterB == b.mIds.end()) break;
            if (++iterC == c.mIds.end()) break;
        }

        if ( *iterA < *iterB && ++iterA == a.mIds.end() ) break;
        if ( *iterA < *iterC && ++iterA == a.mIds.end() ) break;

        if ( *iterB < *iterC && ++iterB == b.mIds.end() ) break;
        if ( *iterB < *iterA && ++iterB == b.mIds.end() ) break;

        if ( *iterC < *iterA && ++iterC == c.mIds.end() ) break;
        if ( *iterC < *iterB && ++iterC == c.mIds.end() ) break;
    }

    //--------------------------------------------------------//

    result.shrink_to_fit();

    return result;
}

//============================================================================//

dop::Group dop::reduce(const dop::Group& a, const dop::Group& b, const dop::Group& c, const dop::Group& d)
{
    const auto reserve = maths::min(a.size(), b.size(), c.size(), d.size());
    dop::Group result; result.reserve(reserve);

    if (reserve == 0u) return result;

    dop::sort(a); dop::sort(b); dop::sort(c); dop::sort(d);

    //--------------------------------------------------------//

    for ( auto iterA = a.mIds.begin(), iterB = b.mIds.begin(), iterC = c.mIds.begin(), iterD = d.mIds.begin();; )
    {
        if (*iterA == *iterB && *iterA == *iterC && *iterA == *iterD)
        {
            result.mIds.push_back(*iterA);
            if (++iterA == a.mIds.end()) break;
            if (++iterB == b.mIds.end()) break;
            if (++iterC == c.mIds.end()) break;
            if (++iterD == d.mIds.end()) break;
        }

        if ( *iterA < *iterB && ++iterA == a.mIds.end() ) break;
        if ( *iterA < *iterC && ++iterA == a.mIds.end() ) break;
        if ( *iterA < *iterD && ++iterA == a.mIds.end() ) break;

        if ( *iterB < *iterC && ++iterB == b.mIds.end() ) break;
        if ( *iterB < *iterD && ++iterB == b.mIds.end() ) break;
        if ( *iterB < *iterA && ++iterB == b.mIds.end() ) break;

        if ( *iterC < *iterD && ++iterC == c.mIds.end() ) break;
        if ( *iterC < *iterA && ++iterC == c.mIds.end() ) break;
        if ( *iterC < *iterB && ++iterC == c.mIds.end() ) break;

        if ( *iterD < *iterA && ++iterD == d.mIds.end() ) break;
        if ( *iterD < *iterB && ++iterD == d.mIds.end() ) break;
        if ( *iterD < *iterC && ++iterD == d.mIds.end() ) break;
    }

    //--------------------------------------------------------//

    result.shrink_to_fit();

    return result;
}
