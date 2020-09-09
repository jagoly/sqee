// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/debug/Assert.hpp>

#include <memory>

namespace sq {

//============================================================================//

/// For internal use within a @ref ResourceCache.
/// @tparam Type type of the resource

template <class Type>
class Resource final : private NonCopyable
{
protected: //=================================================//

    template <class T> friend class Handle;
    template <class K, class T> friend class ResourceCache;

    //--------------------------------------------------------//

    void decrement() { if (--count == 0u) uptr.reset(); }

    bool loaded() const { return uptr != nullptr; }

    //--------------------------------------------------------//

    std::unique_ptr<Type> uptr;
    size_t count = 0u;
};

//============================================================================//

/// Provides reference counted access to a @ref Resource.
/// @tparam Type type of the resource

template <class Type>
class Handle final // Copyable
{
public: //====================================================//

    static_assert(std::is_nothrow_destructible_v<Type>);

    /// Default Constructor.
    Handle() noexcept : mResPtr(nullptr) {}

    /// Copy Constructor.
    Handle(const Handle& other) noexcept { if ((mResPtr = other.mResPtr)) ++mResPtr->count; }

    /// Assignment Operator.
    Handle& operator=(Handle other) noexcept { std::swap(mResPtr, other.mResPtr); return *this; }

    /// Destructor.
    ~Handle() noexcept { if (mResPtr != nullptr) mResPtr->decrement(); }

    //--------------------------------------------------------//

    /// Reset the resource pointer to null.
    void set_null() { this->~Handle(); mResPtr = nullptr; }

    /// Check if the resource pointer is not null.
    bool check() const { return mResPtr != nullptr; }

    //--------------------------------------------------------//

    /// Equality Operator.
    bool operator==(const Handle& other) const { return mResPtr == other.mResPtr; }

    /// Inequality Operator.
    bool operator!=(const Handle& other) const { return mResPtr != other.mResPtr; }

    /// Comparison Operator.
    bool operator<(const Handle& other) const { return mResPtr < other.mResPtr; }

    //--------------------------------------------------------//

    const Type* operator->() const
    {
        SQASSERT(mResPtr != nullptr, "null resource error");
        return mResPtr->uptr.get();
    }

    const Type& get() const
    {
        SQASSERT(mResPtr != nullptr, "null resource error");
        return *mResPtr->uptr;
    }

    //template <class Key>
    //const Key& get_key() const
    //{
    //    SQASSERT(mResPtr != nullptr, "null resource error");
    //    return *std::prev(reinterpret_cast<const Key*>(mResPtr));
    //}

protected: //=================================================//

    template <class K, class T> friend class ResourceCache;

    Handle(Resource<Type>& resource)
    {
        mResPtr = &resource;
        ++mResPtr->count;
    }

    Resource<Type>* mResPtr;
};

//============================================================================//

} // namespace sq
