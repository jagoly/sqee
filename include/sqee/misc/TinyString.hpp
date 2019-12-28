#pragma once

#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Builtins.hpp>

namespace sq {

//============================================================================//

/// Fast static size string type, useful for keys.
class TinyString final
{
public: //====================================================//

    using Traits = std::char_traits<char>;

    static constexpr size_t capacity() noexcept { return 15u; }

    //--------------------------------------------------------//

    constexpr TinyString() noexcept = default;

    constexpr TinyString(const TinyString& other) noexcept = default;

    //--------------------------------------------------------//

    constexpr TinyString(const char* str) noexcept
    {
        SQASSERT(Traits::length(str) < 15u, "string literal too long");
        // won't be constexpr until c++20 :(
        //Traits::copy(mData, str, Traits::length(str));
        const size_t len = Traits::length(str);
        for (size_t i = 0; i < len; ++i) mData[i] = str[i];
    }

    constexpr TinyString(const char* str, size_t len) noexcept
    {
        SQASSERT(len <= 15u, "string or string_view too long");
        // won't be constexpr until c++20 :(
        //Traits::copy(mData, str, len);
        for (size_t i = 0; i < len; ++i) mData[i] = str[i];
    }

    //--------------------------------------------------------//

    constexpr bool operator==(const TinyString& other) const noexcept
    {
        return Traits::compare(mData, other.mData, 16u) == 0;
    }

    constexpr bool operator!=(const TinyString& other) const noexcept
    {
        return Traits::compare(mData, other.mData, 16u) != 0;
    }

    constexpr bool operator<(const TinyString& other) const noexcept
    {
        return Traits::compare(mData, other.mData, 16u) == -1;
    }

    constexpr bool operator>(const TinyString& other) const noexcept
    {
        return Traits::compare(mData, other.mData, 16u) == +1;
    }

    //--------------------------------------------------------//

    bool operator==(const String& other) const noexcept
    {
        return other.compare(mData) == 0;
    }

    bool operator!=(const String& other) const noexcept
    {
        return other.compare(mData) != 0;
    }

    bool operator<(const String& other) const noexcept
    {
        return other.compare(mData) == -1;
    }

    bool operator>(const String& other) const noexcept
    {
        return other.compare(mData) == +1;
    }

    //--------------------------------------------------------//

    constexpr bool empty() const noexcept { return mData[0] == '\0'; }

    constexpr void clear() noexcept { mData[0] = '\0'; }

    constexpr size_t length() const noexcept { return Traits::length(mData); }

    constexpr const char* c_str() const noexcept { return mData; }

    constexpr char* data() noexcept { return mData; }

    //--------------------------------------------------------//

    constexpr TinyString(const StringView& sv) noexcept : TinyString(sv.data(), sv.size()) {}

    TinyString(const String& str) noexcept : TinyString(str.data(), str.size()) {}

    operator String() const { return String(mData); }

private: //===================================================//

    char mData[16u] {};
};

inline const char* to_c_string(const TinyString& arg)
{
    return arg.c_str();
}

inline std::ostream& operator<<(std::ostream& os, const TinyString& arg) { return os << arg.c_str(); }

namespace builtins { using sq::TinyString; }

//============================================================================//

} // namespace sq
