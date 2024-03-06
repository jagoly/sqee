#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace sq { //###############################################################

/// Fast stack only string type, useful for keys.
template <size_t Capacity>
class StackString final
{
public: //======================================================

    using Traits = std::char_traits<char>;

    static constexpr size_t capacity() { return Capacity; }

    static constexpr size_t buffer_size() { return Capacity + 1; }

    //----------------------------------------------------------

    constexpr char* data() { return mData; }

    constexpr const char* data() const { return mData; }

    constexpr const char* c_str() const { return mData; }

    constexpr size_t length() const { return Traits::length(mData); }

    constexpr bool empty() const { return *mData == '\0'; }

    constexpr void clear() { *this = StackString(); }

    constexpr const char* begin() const { return data(); }

    constexpr const char* end() const { return data() + length(); }

    //----------------------------------------------------------

    constexpr StackString() noexcept = default;

    constexpr StackString(const StackString&) noexcept = default;
    constexpr StackString(StackString&&) noexcept = default;

    constexpr StackString& operator=(const StackString&) noexcept = default;
    constexpr StackString& operator=(StackString&&) noexcept = default;

    //----------------------------------------------------------

    template <size_t BufSize>
    constexpr StackString(const char(&chars)[BufSize]) noexcept
    {
        static_assert(BufSize <= buffer_size(), "literal too long");

        Traits::copy(mData, chars, BufSize - 1);
    }

    template <std::same_as<char> CharT>
    constexpr StackString(const CharT* const& cstr)
    {
        const size_t numChars = Traits::length(cstr);

        if (numChars > Capacity)
            throw std::invalid_argument("c string too long");

        Traits::copy(mData, cstr, numChars);
    }

    constexpr StackString(const std::string_view& sv)
    {
        if (sv.length() > Capacity)
            throw std::invalid_argument("string_view too long");

        Traits::copy(mData, sv.data(), sv.length());
    }

    constexpr StackString(const std::string& str)
    {
        if (str.length() > Capacity)
            throw std::invalid_argument("string too long");

        Traits::copy(mData, str.data(), str.length());
    }

    //----------------------------------------------------------

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

    template <std::same_as<char> CharT>
    constexpr int compare(const CharT* const& cstr) const
    {
        // simplified strcmp
        auto ptrL = mData, ptrR = cstr;
        unsigned char charL, charR;
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
        unsigned char charL, charR;
        while (ptrR != ptrEnd)
        {
            charL = static_cast<unsigned char>(*ptrL++);
            charR = static_cast<unsigned char>(*ptrR++);
            if (charL != charR)
                return charL - charR;
        }
        return *ptrL;
    }

    constexpr int compare(const std::string& str) const
    {
        // null terminated algo is a tiny bit faster
        return compare(str.c_str());
    }

    //----------------------------------------------------------

    constexpr auto operator<=>(const StackString& other) const { return compare(other) <=> 0; }
    constexpr bool operator==(const StackString& other) const { return compare(other) == 0; }

    template <size_t BufSize>
    constexpr auto operator<=>(const char(&chars)[BufSize]) const { return compare(chars) <=> 0; }
    template <size_t BufSize>
    constexpr bool operator==(const char(&chars)[BufSize]) const { return compare(chars) == 0; }

    template <std::same_as<char> CharT>
    constexpr auto operator<=>(const CharT* const& cstr) const { return compare(cstr) <=> 0; }
    template <std::same_as<char> CharT>
    constexpr bool operator==(const CharT* const& cstr) const { return compare(cstr) == 0; }

    constexpr auto operator<=>(const std::string_view& sv) const { return compare(sv) <=> 0; }
    constexpr bool operator==(const std::string_view& sv) const { return compare(sv) == 0; }

    constexpr auto operator<=>(const std::string& str) const { return compare(str) <=> 0; }
    constexpr bool operator==(const std::string& str) const { return compare(str) == 0; }

    //----------------------------------------------------------

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

    //----------------------------------------------------------

    constexpr operator std::string_view() const noexcept
    {
        return std::string_view(mData, length());
    }

    //----------------------------------------------------------

    // public so that the class is structural, and thus usable for non-type template params
    char mData[buffer_size()] {};
};

//==============================================================================

// deduction guide for compile time string stuff
template <size_t BufSize>
StackString(const char(&chars)[BufSize]) -> StackString<BufSize-1>;

} // namespace sq ##############################################################

#include <fmt/format.h>

template <size_t Capacity>
struct fmt::formatter<sq::StackString<Capacity>>
{
    fmt::formatter<fmt::string_view> base;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return base.parse(ctx);
    }

    template <class FormatContext>
    constexpr FormatContext::iterator format(const sq::StackString<Capacity>& ss, FormatContext& ctx) const
    {
        return base.format(fmt::string_view(ss.data(), ss.length()), ctx);
    }
};

#include <functional>

template <size_t Capacity>
struct std::hash<sq::StackString<Capacity>>
{
    size_t operator()(const sq::StackString<Capacity>& ss) const noexcept
    {
        auto sv = std::string_view(ss.data(), ss.buffer_size());
        return hash<std::string_view>()(sv);
    }
};
