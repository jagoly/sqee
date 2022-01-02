// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace sq {

//============================================================================//

/// Fast stack only string type, useful for keys.
template <size_t Capacity>
class StackString final
{
public: //====================================================//

    using Traits = std::char_traits<char>;

    static constexpr size_t capacity() { return Capacity; }

    static constexpr size_t buffer_size() { return Capacity + 1u; }

    //--------------------------------------------------------//

    constexpr char* data() { return mData; }

    constexpr const char* data() const { return mData; }

    constexpr const char* c_str() const { return mData; }

    constexpr size_t length() const { return Traits::length(mData); }

    constexpr bool empty() const { return *mData == '\0'; }

    constexpr void clear() { *this = StackString(); }

    //--------------------------------------------------------//

    constexpr StackString() noexcept = default;

    constexpr StackString(const StackString&) noexcept = default;
    constexpr StackString(StackString&&) noexcept = default;

    constexpr StackString& operator=(const StackString&) noexcept = default;
    constexpr StackString& operator=(StackString&&) noexcept = default;

    //--------------------------------------------------------//

    template <size_t BufSize>
    constexpr StackString(const char(&chars)[BufSize]) noexcept
    {
        static_assert(BufSize <= buffer_size(), "literal too long");

        for (size_t i = 0u; i < BufSize - 1u; ++i)
            mData[i] = chars[i];
        //Traits::copy(mData, chars, BufSize - 1u);
    }

    template <class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>>
    constexpr StackString(const CharT* const& cstr)
    {
        const size_t numChars = Traits::length(cstr);

        if (numChars > Capacity)
            throw std::invalid_argument("c string too long");

        for (size_t i = 0u; i < numChars; ++i)
            mData[i] = cstr[i];
        //Traits::copy(mData, cstr, numChars);
    }

    constexpr StackString(const std::string_view& sv)
    {
        if (sv.length() > Capacity)
            throw std::invalid_argument("string_view too long");

        for (size_t i = 0u; i < sv.length(); ++i)
            mData[i] = sv.data()[i];
        //Traits::copy(mData, sv.data(), sv.length());
    }

    /*constexpr*/ StackString(const std::string& str)
    {
        if (str.length() > Capacity)
            throw std::invalid_argument("string too long");

        Traits::copy(mData, str.data(), str.length());
    }

    //--------------------------------------------------------//

    constexpr int compare(const StackString& other) const
    {
        return Traits::compare(mData, other.mData, buffer_size());
    }

    template <size_t BufSize>
    constexpr int compare(const char(&chars)[BufSize]) const
    {
        constexpr size_t n = BufSize < buffer_size() ? BufSize : buffer_size();
        return Traits::compare(mData, chars, n);
    }

    template <class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>>
    constexpr int compare(const CharT* const& cstr) const
    {
        // simplified strcmp
        auto ptrL = mData, ptrR = cstr;
        unsigned char charL {}, charR {};
        do
        {
            charL = static_cast<unsigned char>(*ptrL++);
            charR = static_cast<unsigned char>(*ptrR++);
        }
        while (charL != '\0' && charL == charR);
        return charL - charR;
    }

    constexpr int compare(const std::string_view& sv) const
    {
        // simplified strncmp
        auto ptrL = mData, ptrR = sv.data();
        const auto ptrEnd = sv.data() + sv.length();
        unsigned char charL {}, charR {};
        while (ptrR != ptrEnd)
        {
            charL = static_cast<unsigned char>(*ptrL++);
            charR = static_cast<unsigned char>(*ptrR++);
            if (charL != charR)
                return charL - charR;
        }
        return *ptrL;
    }

    /*constexpr*/ int compare(const std::string& str) const
    {
        // null terminated algo is a tiny bit faster
        return compare(str.c_str());
    }

    //--------------------------------------------------------//

    constexpr bool starts_with(std::string_view prefix) const
    {
        if (prefix.length() > Capacity) return false;
        return Traits::compare(mData, prefix.data(), prefix.length()) == 0;
    }

    constexpr bool ends_with(std::string_view suffix) const
    {
        if (suffix.length() > Capacity) return false;
        return Traits::compare(mData + Capacity - suffix.length(), suffix.data(), suffix.length()) == 0;
    }

    //constexpr std::string_view substr(size_t pos, size_t count) const
    //{
    //    // let string_view do the bounds checking for us
    //    return std::string_view(mData, length()).substr(pos, count);
    //}

    //--------------------------------------------------------//

    constexpr operator std::string_view() const noexcept
    {
        return std::string_view(mData, length());
    }

private: //===================================================//

    char mData[buffer_size()] {}; // initialise with zeros
};

//============================================================================//

// plz c++20 save me

template <size_t Capacity> constexpr
bool operator==(const StackString<Capacity>& lhs, const StackString<Capacity>& rhs) { return lhs.compare(rhs) == 0; }
template <size_t Capacity> constexpr
bool operator!=(const StackString<Capacity>& lhs, const StackString<Capacity>& rhs) { return lhs.compare(rhs) != 0; }
template <size_t Capacity> constexpr
bool operator<(const StackString<Capacity>& lhs, const StackString<Capacity>& rhs) { return lhs.compare(rhs) < 0; }
template <size_t Capacity> constexpr
bool operator>(const StackString<Capacity>& lhs, const StackString<Capacity>& rhs) { return lhs.compare(rhs) > 0; }

template <size_t Capacity, size_t BufSize> constexpr
bool operator==(const StackString<Capacity>& lhs, const char(&rhs)[BufSize]) { return lhs.compare(rhs) == 0; }
template <size_t Capacity, size_t BufSize> constexpr
bool operator!=(const StackString<Capacity>& lhs, const char(&rhs)[BufSize]) { return lhs.compare(rhs) != 0; }
template <size_t Capacity, size_t BufSize> constexpr
bool operator<(const StackString<Capacity>& lhs, const char(&rhs)[BufSize]) { return lhs.compare(rhs) < 0; }
template <size_t Capacity, size_t BufSize> constexpr
bool operator>(const StackString<Capacity>& lhs, const char(&rhs)[BufSize]) { return lhs.compare(rhs) > 0; }
template <size_t Capacity, size_t BufSize> constexpr
bool operator==(const char(&lhs)[BufSize], const StackString<Capacity>& rhs) { return rhs.compare(lhs) == 0; }
template <size_t Capacity, size_t BufSize> constexpr
bool operator!=(const char(&lhs)[BufSize], const StackString<Capacity>& rhs) { return rhs.compare(lhs) != 0; }
template <size_t Capacity, size_t BufSize> constexpr
bool operator<(const char(&lhs)[BufSize], const StackString<Capacity>& rhs) { return rhs.compare(lhs) > 0; }
template <size_t Capacity, size_t BufSize> constexpr
bool operator>(const char(&lhs)[BufSize], const StackString<Capacity>& rhs) { return rhs.compare(lhs) < 0; }

template <size_t Capacity, class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>> constexpr
bool operator==(const StackString<Capacity>& lhs, const CharT* const& rhs) { return lhs.compare(rhs) == 0; }
template <size_t Capacity, class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>> constexpr
bool operator!=(const StackString<Capacity>& lhs, const CharT* const& rhs) { return lhs.compare(rhs) != 0; }
template <size_t Capacity, class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>> constexpr
bool operator<(const StackString<Capacity>& lhs, const CharT* const& rhs) { return lhs.compare(rhs) < 0; }
template <size_t Capacity, class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>> constexpr
bool operator>(const StackString<Capacity>& lhs, const CharT* const& rhs) { return lhs.compare(rhs) > 0; }
template <size_t Capacity, class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>> constexpr
bool operator==(const CharT* const& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) == 0; }
template <size_t Capacity, class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>> constexpr
bool operator!=(const CharT* const& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) != 0; }
template <size_t Capacity, class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>> constexpr
bool operator<(const CharT* const& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) > 0; }
template <size_t Capacity, class CharT, class = std::enable_if_t<std::is_same_v<CharT, char>>> constexpr
bool operator>(const CharT* const& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) < 0; }

template <size_t Capacity> constexpr
bool operator==(const StackString<Capacity>& lhs, const std::string_view& rhs) { return lhs.compare(rhs) == 0; }
template <size_t Capacity> constexpr
bool operator!=(const StackString<Capacity>& lhs, const std::string_view& rhs) { return lhs.compare(rhs) != 0; }
template <size_t Capacity> constexpr
bool operator<(const StackString<Capacity>& lhs, const std::string_view& rhs) { return lhs.compare(rhs) < 0; }
template <size_t Capacity> constexpr
bool operator>(const StackString<Capacity>& lhs, const std::string_view& rhs) { return lhs.compare(rhs) > 0; }
template <size_t Capacity> constexpr
bool operator==(const std::string_view& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) == 0; }
template <size_t Capacity> constexpr
bool operator!=(const std::string_view& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) != 0; }
template <size_t Capacity> constexpr
bool operator<(const std::string_view& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) > 0; }
template <size_t Capacity> constexpr
bool operator>(const std::string_view& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) < 0; }

template <size_t Capacity> inline
bool operator==(const StackString<Capacity>& lhs, const std::string& rhs) { return lhs.compare(rhs) == 0; }
template <size_t Capacity> inline
bool operator!=(const StackString<Capacity>& lhs, const std::string& rhs) { return lhs.compare(rhs) != 0; }
template <size_t Capacity> inline
bool operator<(const StackString<Capacity>& lhs, const std::string& rhs) { return lhs.compare(rhs) < 0; }
template <size_t Capacity> inline
bool operator>(const StackString<Capacity>& lhs, const std::string& rhs) { return lhs.compare(rhs) > 0; }
template <size_t Capacity> inline
bool operator==(const std::string& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) == 0; }
template <size_t Capacity> inline
bool operator!=(const std::string& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) != 0; }
template <size_t Capacity> inline
bool operator<(const std::string& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) > 0; }
template <size_t Capacity> inline
bool operator>(const std::string& lhs, const StackString<Capacity>& rhs) { return rhs.compare(lhs) < 0; }

//============================================================================//

template <size_t Capacity>
constexpr const char* type_to_string(StackString<Capacity>)
{
    if constexpr (Capacity == 15u) return "TinyString";
    if constexpr (Capacity == 23u) return "SmallString";
    return "StackString";
}

//============================================================================//

} // namespace sq

template <size_t Capacity>
struct std::hash<sq::StackString<Capacity>>
{
    size_t operator()(const sq::StackString<Capacity>& ss) const noexcept
    {
        auto sv = std::string_view(ss.data(), ss.buffer_size());
        return hash<std::string_view>()(sv);
    }
};
