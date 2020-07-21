#pragma once

#include <sqee/debug/Assert.hpp>

#include <cstring>     // strncmp
#include <string>      // string, char_traits
#include <string_view> // string_view
#include <utility>     // hash

namespace sq {

//============================================================================//

/// Fast stack only string type, useful for keys.
///
/// If size is less than capacity, then the extra bytes are guarenteed to be
/// filled with zeros, which enables faster comparisons.
///
template <size_t Capacity>
class FixedCapacityString final
{
public: //====================================================//

    using Traits = std::char_traits<char>;

    static constexpr size_t capacity() noexcept { return Capacity; }
    static constexpr size_t buffer_size() noexcept { return Capacity + 1u; }

    // can do some optimizations when this is a multiple of 8
    static_assert(buffer_size() % 8u == 0u, "invalid capacity");

    //--------------------------------------------------------//

    constexpr FixedCapacityString() noexcept = default;

    constexpr FixedCapacityString(const FixedCapacityString& other) noexcept = default;

    //--------------------------------------------------------//

    constexpr FixedCapacityString(const char* cstr) noexcept
    {
        SQASSERT(Traits::length(cstr) <= Capacity, "string literal too long");
        //Traits::copy(mData, str, Traits::length(str));
        const size_t len = Traits::length(cstr);
        for (size_t i = 0; i < len; ++i) mData[i] = cstr[i];
    }

    constexpr FixedCapacityString(const std::string_view& sv) noexcept
    {
        SQASSERT(sv.length() <= 15u, "string_view too long");
        //Traits::copy(mData, sv.data(), sv.length());
        for (size_t i = 0; i < sv.length(); ++i) mData[i] = sv[i];
    }

    constexpr FixedCapacityString(const std::string& str) noexcept
    {
        SQASSERT(str.length() <= 15u, "string too long");
        Traits::copy(mData, str.data(), str.length());
        for (size_t i = 0; i < str.length(); ++i) mData[i] = str[i];
    }

    //--------------------------------------------------------//

    constexpr int compare(const FixedCapacityString& other) const noexcept
    {
        return Traits::compare(mData, other.mData, buffer_size());
    }

    constexpr int compare(const char* other) const noexcept
    {
        //if constexpr (std::is_constant_evaluated())
        //{
            for (size_t i = 0u; i < buffer_size(); ++i)
            {
                if (mData[i] < other[i]) return -1;
                if (mData[i] > other[i]) return +1;
                if (mData[i] == '\0' && other[i] == '\0') return 0;
            }
            return 0;
        //}
        //else return std::strncmp(mData, other, buffer_size());
    }

    constexpr int compare(const std::string_view& other) const noexcept
    {
        const int cmpResult = Traits::compare(mData, other.data(), std::min(capacity(), other.length()));
        if (cmpResult != 0) return cmpResult;

        const size_t len = length();
        if (len < other.length()) return -1;
        if (len > other.length()) return +1;
        return 0;
    }

    constexpr bool operator==(const FixedCapacityString& other) const noexcept { return compare(other) == 0; }
    constexpr bool operator!=(const FixedCapacityString& other) const noexcept { return compare(other) != 0; }
    constexpr bool operator<(const FixedCapacityString& other) const noexcept { return compare(other) < 0; }
    constexpr bool operator>(const FixedCapacityString& other) const noexcept { return compare(other) > 0; }

    constexpr bool operator==(const char* other) const noexcept { return compare(other) == 0; }
    constexpr bool operator!=(const char* other) const noexcept { return compare(other) != 0; }
    constexpr bool operator<(const char* other) const noexcept { return compare(other) < 0; }
    constexpr bool operator>(const char* other) const noexcept { return compare(other) > 0; }

    constexpr bool operator==(const std::string_view& other) const noexcept { return compare(other) == 0; }
    constexpr bool operator!=(const std::string_view& other) const noexcept { return compare(other) != 0; }
    constexpr bool operator<(const std::string_view& other) const noexcept { return compare(other) < 0; }
    constexpr bool operator>(const std::string_view& other) const noexcept { return compare(other) > 0; }

    //--------------------------------------------------------//

    constexpr bool empty() const noexcept { return mData[0] == '\0'; }

    constexpr void clear() noexcept { *this = {}; }

    constexpr size_t length() const noexcept { return Traits::length(mData); }

    constexpr const char* c_str() const noexcept { return mData; }

    constexpr char* data() noexcept { return mData; }

    //--------------------------------------------------------//

    // not having this overload is annoying, but MSVC has trouble disambiguating it
    //operator std::string() const { return std::string(mData); }

    constexpr operator const char*() const noexcept { return mData; }

private: //===================================================//

    char mData[buffer_size()] {};
};

//============================================================================//

using SmallString = FixedCapacityString<23u>;
using TinyString = FixedCapacityString<15u>;

inline const char* to_c_string(const SmallString& arg) { return arg.c_str(); }
inline const char* to_c_string(const TinyString& arg) { return arg.c_str(); }

namespace builtins { using sq::SmallString; using sq::TinyString; }

//============================================================================//

} // namespace sq

//============================================================================//

namespace std {

template <size_t Capacity>
struct hash<sq::FixedCapacityString<Capacity>>
{
    size_t operator()(const sq::FixedCapacityString<Capacity>& fcs) const
    {
        return hash<std::string_view>()(std::string_view(fcs.c_str()));
    }
};

} // namespace std
