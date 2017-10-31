#pragma once

#include <sqee/setup.hpp>

namespace sq {

//============================================================================//

template <class T> struct is_flags : public std::false_type {};
template <class T> constexpr bool is_flags_v = is_flags<T>::value;

//============================================================================//

template <class T> class Flags
{
public: //====================================================//

    static_assert(is_flags_v<T>, "SQEE_ENABLE_FLAGS_FOR_ENUM not called for this type.");

    //--------------------------------------------------------//

    using enum_type = std::decay_t<T>;
    using underlying_type = std::underlying_type_t<enum_type>;
    using impl_type = std::make_unsigned_t<underlying_type>;

    //--------------------------------------------------------//

    constexpr Flags() noexcept = default;
    constexpr Flags(const Flags& other) noexcept = default;
    constexpr Flags& operator=(const Flags& other) noexcept = default;
    constexpr Flags(Flags&& other) noexcept = default;
    constexpr Flags& operator=(Flags&& other) noexcept = default;

    //--------------------------------------------------------//

    constexpr Flags(enum_type flag) noexcept
    {
        mBits = static_cast<impl_type>(flag);
    }

    constexpr Flags& operator=(enum_type flag) noexcept
    {
        return ( *this = Flags(flag) );
    }

    //--------------------------------------------------------//

    template <class... Ts>
    constexpr Flags(T flag, Ts... flags) noexcept
    {
        static_assert((std::is_same_v<T, Ts> || ...));

        mBits = (static_cast<impl_type>(flags) | ...);
        mBits |= static_cast<impl_type>(flag);
    }

    //--------------------------------------------------------//

    Flags(std::initializer_list<enum_type> flags) noexcept
    {
        for (const enum_type flag : flags)
            mBits |= static_cast<impl_type>(flag);
    }

    Flags& operator=(std::initializer_list<enum_type> flags) noexcept
    {
        return ( *this = Flags(flags) );
    }

    Flags(std::initializer_list<const enum_type> flags) noexcept
    {
        for (const enum_type flag : flags)
            mBits |= static_cast<impl_type>(flag);
    }

    Flags& operator=(std::initializer_list<const enum_type> flags) noexcept
    {
        return ( *this = Flags(flags) );
    }

    //--------------------------------------------------------//

    constexpr explicit operator bool() const noexcept { return mBits; }

    constexpr Flags operator~() const noexcept { return Flags(~mBits); }

    //--------------------------------------------------------//

    constexpr Flags& operator|=(Flags rhs) noexcept
    { mBits |= rhs.mBits; return *this; }

    constexpr Flags& operator&=(Flags rhs) noexcept
    { mBits &= rhs.mBits; return *this; }

    constexpr Flags& operator^=(Flags rhs) noexcept
    { mBits ^= rhs.mBits; return *this; }

    constexpr Flags& operator|=(enum_type rhs) noexcept
    { mBits |= static_cast<impl_type>(rhs); return *this; }

    constexpr Flags& operator&=(enum_type rhs) noexcept
    { mBits &= static_cast<impl_type>(rhs); return *this; }

    constexpr Flags& operator^=(enum_type rhs) noexcept
    { mBits ^= static_cast<impl_type>(rhs); return *this; }

    //--------------------------------------------------------//

    friend constexpr bool operator==(Flags lhs, Flags rhs)
    { return lhs.mBits == rhs.mBits; }

    friend constexpr bool operator!=(Flags lhs, Flags rhs)
    { return lhs.mBits != rhs.mBits; }

    friend constexpr Flags operator|(Flags lhs, Flags rhs) noexcept
    { return Flags { lhs.mBits | rhs.mBits }; }

    friend constexpr Flags operator&(Flags lhs, Flags rhs) noexcept
    { return Flags { lhs.mBits & rhs.mBits }; }

    friend constexpr Flags operator^(Flags lhs, Flags rhs) noexcept
    { return Flags { lhs.mBits ^ rhs.mBits }; }

    //--------------------------------------------------------//

    constexpr underlying_type get_underlying() const noexcept
    {
        return static_cast<underlying_type>(mBits);
    }

    constexpr void set_underlying(underlying_type value) noexcept
    {
        value = static_cast<impl_type>(value);
    }

private: //===================================================//

    constexpr explicit Flags(impl_type bits) noexcept : mBits(bits) {}

    impl_type mBits {}; // initialise to zero
};

template <class T> using FlagSet = Flags<T>;

//============================================================================//

} // namespace sq

//============================================================================//

template <class T> constexpr auto operator|(T lhs, T rhs) noexcept
-> std::enable_if_t<sq::is_flags_v<T>, sq::Flags<T>> { return sq::Flags<T>(lhs) | rhs; }

template <class T> constexpr auto operator&(T lhs, T rhs) noexcept
-> std::enable_if_t<sq::is_flags_v<T>, sq::Flags<T>> { return sq::Flags<T>(lhs) & rhs; }

template <class T> constexpr auto operator^(T lhs, T rhs) noexcept
-> std::enable_if_t<sq::is_flags_v<T>, sq::Flags<T>> { return sq::Flags<T>(lhs) ^ rhs; }

//============================================================================//

#define SQEE_ALLOW_FLAGS_FOR_ENUM(Name) \
namespace sq { template<> struct is_flags<Name> : std::true_type {}; }
