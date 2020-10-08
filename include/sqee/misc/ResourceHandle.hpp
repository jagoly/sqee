// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/debug/Assert.hpp>

namespace sq {

//============================================================================//

/// For internal use by ResourceCache and Handle.
template <class Key, class Type>
struct Resource final : private NonCopyable
{
    const Key* key;
    std::unique_ptr<Type> data;
    size_t count;
};

//============================================================================//

/// Provides reference counted access to a resource.
template <class Key, class Type>
class Handle final
{
public: //====================================================

    Handle(std::nullptr_t = nullptr) : mResourcePtr(nullptr) {}

    Handle(Resource<Key, Type>& resource) : mResourcePtr(&resource) { ++mResourcePtr->count; }

    Handle(const Handle& other) = default;

    Handle& operator=(const Handle& other) = default;

    ~Handle() { if (mResourcePtr) --mResourcePtr->count; }

    //--------------------------------------------------------//

    const Type* operator->() const
    {
        SQASSERT(mResourcePtr, "null handle error");
        return mResourcePtr->data.get();
    }

    const Type& operator*() const
    {
        SQASSERT(mResourcePtr, "null handle error");
        return *mResourcePtr->data;
    }

    const Type& get() const
    {
        SQASSERT(mResourcePtr, "null handle error");
        return *mResourcePtr->data;
    }

    const Key& get_key() const
    {
        SQASSERT(mResourcePtr, "null handle error");
        return *mResourcePtr->key;
    }

    //--------------------------------------------------------//

    /// Equality Operator.
    bool operator==(const Handle& other) const { return mResourcePtr == other.mResourcePtr; }

    /// Inequality Operator.
    bool operator!=(const Handle& other) const { return mResourcePtr != other.mResourcePtr; }

private: //===================================================//

    Resource<Key, Type>* mResourcePtr;
};

//============================================================================//

} // namespace sq
