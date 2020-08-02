// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <fmt/format.h>

#include <cstring>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace sq {

//============================================================================//

/// Fast stack only string type, useful for keys.
///
/// Unused bytes are guarenteed to be zero, which enables faster comparisons.
///
template <size_t Capacity>
class StackString final
{
public: //====================================================//

    using Traits = std::char_traits<char>;

    static constexpr size_t capacity() noexcept { return Capacity; }
    static constexpr size_t buffer_size() noexcept { return Capacity + 1u; }

    // can do some optimizations when this is a multiple of 8
    static_assert(buffer_size() % 8u == 0u, "invalid capacity");

    //--------------------------------------------------------//

    constexpr StackString() noexcept = default;

    constexpr StackString(const StackString& other) noexcept = default;

    //--------------------------------------------------------//

    constexpr StackString(const char* cstr)
    {
        if (Traits::length(cstr) > Capacity)
            throw std::invalid_argument("string literal too long");
        //Traits::copy(mData, cstr, Traits::length(cstr));
        const size_t len = Traits::length(cstr);
        for (size_t i = 0; i < len; ++i) mData[i] = cstr[i];
    }

    constexpr StackString(const std::string_view& sv)
    {
        if (sv.length() > Capacity)
            throw std::invalid_argument("string_view too long");
        //Traits::copy(mData, sv.data(), sv.length());
        for (size_t i = 0; i < sv.length(); ++i) mData[i] = sv[i];
    }

    StackString(const std::string& str)
    {
        if (str.length() > Capacity)
            throw std::invalid_argument("string too long");
        Traits::copy(mData, str.data(), str.length());
    }

    //--------------------------------------------------------//

    constexpr int compare(const StackString& other) const noexcept
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

    int compare(const std::string& other) const noexcept
    {
        return std::strncmp(mData, other.c_str(), buffer_size());
    }

    //--------------------------------------------------------//

    constexpr bool operator==(const StackString& other) const noexcept { return compare(other) == 0; }
    constexpr bool operator!=(const StackString& other) const noexcept { return compare(other) != 0; }
    constexpr bool operator<(const StackString& other) const noexcept { return compare(other) < 0; }
    constexpr bool operator>(const StackString& other) const noexcept { return compare(other) > 0; }

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

    char mData[buffer_size()] {}; // initialize to zeros
};

//============================================================================//

template <size_t Capacity>
constexpr const char* type_to_string(StackString<Capacity>)
{
    if constexpr (Capacity == 15u) return "TinyString";
    if constexpr (Capacity == 31u) return "SmallString";
    return "StackString<C>";
}

//============================================================================//

} // namespace sq

//============================================================================//

namespace std {

// not having this overload is annoying, but MSVC has trouble disambiguating it
//operator std::string() const { return std::string(mData); }

template <size_t Capacity>
struct hash<sq::StackString<Capacity>>
{
    size_t operator()(const sq::StackString<Capacity>& fcs) const
    {
        return hash<std::string_view>()(std::string_view(fcs.c_str()));
    }
};

} // namespace std
