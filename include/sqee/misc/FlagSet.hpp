#pragma once

#include <sqee/setup.hpp>

//============================================================================//

namespace sq {

template <class Flag> class FlagSet final
{
public: //====================================================//

    constexpr FlagSet() noexcept = default;

    template <class... Flags>
    constexpr FlagSet(Flag flag, Flags... flags) noexcept
    {
        static_assert((std::is_same_v<Flag, Flags> || ...));
        mBits = uint64_t(flag) | (uint64_t(flags) | ...);
    }

    FlagSet(std::initializer_list<Flag> flags)
    {
        for (Flag flag : flags) mBits |= uint64_t(flag);
    }

    // gcc seems picky about this sometimes
    FlagSet(std::initializer_list<const Flag> flags)
    {
        for (Flag flag : flags) mBits |= uint64_t(flag);
    }

    //--------------------------------------------------------//

    constexpr bool check(Flag flag) const noexcept
    { return mBits & uint64_t(flag); }

    constexpr bool operator&(Flag flag) const noexcept
    { return mBits & uint64_t(flag); }

    //--------------------------------------------------------//

    template <class... Flags>
    constexpr bool all_of(Flags... flags) const noexcept
    { return !(~mBits & FlagSet(flags...).mBits); }

    template <class... Flags>
    constexpr bool none_of(Flags... flags) const noexcept
    { return !( mBits & FlagSet(flags...).mBits); }

    template <class... Flags>
    constexpr bool any_of(Flags... flags) const noexcept
    { return  ( mBits & FlagSet(flags...).mBits); }

private: //===================================================//

    constexpr FlagSet(uint64_t bits) noexcept : mBits(bits) {}

    uint64_t mBits = 0u;
};

} // namespace sq
