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

    // methods taking c strings are templates to reduce overload priority
    template <class CharT>
    using EnableIfChar = std::enable_if_t<std::is_same_v<CharT, char>>;

    static constexpr size_t capacity() { return Capacity; }
    static constexpr size_t buffer_size() { return Capacity + 1u; }

    // compiler can better optimise with multiples of 8
    static_assert(buffer_size() % 8u == 0u, "invalid capacity");

    //--------------------------------------------------------//

    constexpr StackString() noexcept = default;

    constexpr StackString(const StackString& other) noexcept = default;

    //--------------------------------------------------------//

    /// Construct from a string literal.
    template <size_t BufSize>
    constexpr StackString(const char(&chars)[BufSize]) noexcept
    {
        static_assert(BufSize <= buffer_size(), "literal too long");

        for (size_t i = 0u; i < BufSize; ++i)
            mData[i] = chars[i];
    }

    /// Construct from a const char ptr.
    template <class CharT, class = EnableIfChar<CharT>>
    StackString(const CharT* const& cstr)
    {
        const size_t numChars = Traits::length(cstr);

        if (numChars > Capacity)
            throw std::invalid_argument("c string too long");

        Traits::copy(mData, cstr, numChars);
    }

    //--------------------------------------------------------//

    StackString(const std::string_view& sv)
    {
        if (sv.length() > Capacity)
            throw std::invalid_argument("string_view too long");

        Traits::copy(mData, sv.data(), sv.length());
    }

    StackString(const std::string& str)
    {
        if (str.length() > Capacity)
            throw std::invalid_argument("string too long");

        Traits::copy(mData, str.data(), str.length());
    }

    //--------------------------------------------------------//

    // todo: compare stack strings with different capacities, for now not needed

    constexpr int compare(const StackString& other) const
    {
        return Traits::compare(mData, other.mData, buffer_size());
    }

    template <size_t BufSize>
    constexpr int compare(const char(&chars)[BufSize]) const
    {
        if constexpr (BufSize >= buffer_size())
            return Traits::compare(mData, chars, buffer_size());

        const int cmp = Traits::compare(mData, chars, BufSize);
        if (cmp != 0) return cmp;

        return int(Traits::length(mData)) - int(BufSize) + 1;
    }

    template <class CharT, class = EnableIfChar<CharT>>
    int compare(const CharT* const& other) const
    {
        return std::strncmp(mData, other, buffer_size());
    }

    int compare(const std::string_view& other) const
    {
        if (other.length() >= buffer_size())
            return Traits::compare(mData, other.data(), buffer_size());

        const int cmp = Traits::compare(mData, other.data(), other.length());
        if (cmp != 0) return cmp;

        return int(Traits::length(mData)) - int(other.length());
    }

    int compare(const std::string& other) const
    {
        return std::strncmp(mData, other.c_str(), buffer_size());
    }

    //--------------------------------------------------------//

    constexpr bool operator==(const StackString& other) const { return compare(other) == 0; }
    constexpr bool operator!=(const StackString& other) const { return compare(other) != 0; }
    constexpr bool operator<(const StackString& other) const { return compare(other) < 0; }
    constexpr bool operator>(const StackString& other) const { return compare(other) > 0; }

    template <size_t BufSize>
    constexpr bool operator==(const char(&other)[BufSize]) const { return compare(other) == 0; }
    template <size_t BufSize>
    constexpr bool operator!=(const char(&other)[BufSize]) const { return compare(other) != 0; }
    template <size_t BufSize>
    constexpr bool operator<(const char(&other)[BufSize]) const { return compare(other) < 0; }
    template <size_t BufSize>
    constexpr bool operator>(const char(&other)[BufSize]) const { return compare(other) > 0; }

    template <class CharT, class = EnableIfChar<CharT>>
    bool operator==(const CharT* const& other) const { return compare(other) == 0; }
    template <class CharT, class = EnableIfChar<CharT>>
    bool operator!=(const CharT* const& other) const { return compare(other) != 0; }
    template <class CharT, class = EnableIfChar<CharT>>
    bool operator<(const CharT* const& other) const { return compare(other) < 0; }
    template <class CharT, class = EnableIfChar<CharT>>
    bool operator>(const CharT* const& other) const { return compare(other) > 0; }

    bool operator==(const std::string_view& other) const { return compare(other) == 0; }
    bool operator!=(const std::string_view& other) const { return compare(other) != 0; }
    bool operator<(const std::string_view& other) const { return compare(other) < 0; }
    bool operator>(const std::string_view& other) const { return compare(other) > 0; }

    bool operator==(const std::string& other) const { return compare(other) == 0; }
    bool operator!=(const std::string& other) const { return compare(other) != 0; }
    bool operator<(const std::string& other) const { return compare(other) < 0; }
    bool operator>(const std::string& other) const { return compare(other) > 0; }

    //--------------------------------------------------------//

    constexpr bool empty() const { return mData[0] == '\0'; }

    constexpr void clear() { *this = {}; }

    constexpr size_t length() const { return Traits::length(mData); }

    constexpr const char* c_str() const { return mData; }

    constexpr char* data() { return mData; }

    //--------------------------------------------------------//

    constexpr operator const char*() const { return mData; }

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

template <size_t Capacity>
struct hash<sq::StackString<Capacity>>
{
    size_t operator()(const sq::StackString<Capacity>& str) const
    {
        return hash<std::string_view>()(std::string_view(str.c_str()));
    }
};

} // namespace std
