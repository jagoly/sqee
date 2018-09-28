// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Builtins.hpp>

namespace sq {

//====== Forward Declarations ================================================//

template <class Key, class Type> class Handle;
template <class Key, class Type> class ResourceCache;

//============================================================================//

/// For internal use within a @ref ResourceCache.
/// @tparam Key hashable key type
/// @tparam Type type of the resource

template <class Key, class Type>
class Resource final : private NonCopyable
{
protected: //=================================================//

    friend class Handle<Key, Type>;
    friend class ResourceCache<Key, Type>;

    //--------------------------------------------------------//

    void decrement() { if (--count == 0u) uptr.reset(); }
    bool loaded() const { return uptr != nullptr; }

    //--------------------------------------------------------//

    UniquePtr<Type> uptr;
    uint count = 0u;

    Key key;
};

//============================================================================//

/// Provides reference counted access to a @ref Resource.
/// @tparam Key hashable key type
/// @tparam Type type of the resource

template <class Key, class Type>
class Handle final
{
public: //====================================================//

    /// Default Constructor.
    Handle() : mResPtr(nullptr) {}

    /// Copy Constructor.
    Handle(const Handle& other) { if ((mResPtr = other.mResPtr)) ++mResPtr->count; }

    /// Assignment Operator.
    Handle& operator=(Handle other) { std::swap(mResPtr, other.mResPtr); return *this; }

    /// Destructor.
    ~Handle() { if (mResPtr != nullptr) mResPtr->decrement(); }

    //--------------------------------------------------------//

    /// Reset the resource pointer to null.
    void set_null() { ~Handle(); mResPtr = nullptr; }

    /// Check if the resource pointer is null.
    bool check() const { return mResPtr != nullptr; }

    //--------------------------------------------------------//

    /// Get the unique key of the resource.
    const Key& get_key() const { return mResPtr ? mResPtr->key : ""; }

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
        SQASSERT(mResPtr != nullptr, "");
        return mResPtr->uptr.get();
    }

    const Type& get() const
    {
        SQASSERT(mResPtr != nullptr, "");
        return *mResPtr->uptr;
    }

protected: //=================================================//

    friend class ResourceCache<Key, Type>;

    //--------------------------------------------------------//

    Handle(Resource<Key, Type>& resource)
    {
        mResPtr = &resource;
        ++mResPtr->count;
    }

    //--------------------------------------------------------//

    Resource<Key, Type>* mResPtr;
};

//============================================================================//

} // namespace sq
