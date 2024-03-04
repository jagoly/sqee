// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <cassert>

namespace sq {

//============================================================================//

/// For internal use by ResourceCache and Handle.
template <class Key, class Type>
struct Resource final
{
    Resource() = default;

    SQEE_COPY_DELETE(Resource)
    SQEE_MOVE_DELETE(Resource)

    const Key* key;
    std::unique_ptr<Type> value;
    std::string error;
    size_t count;
};

//============================================================================//

/// Provides reference counted access to a resource.
template <class Key, class Type>
class Handle final
{
public: //====================================================//

    Handle(std::nullptr_t = nullptr) noexcept
    {
        mResourcePtr = nullptr;
    }

    Handle(Resource<Key, Type>& resource) noexcept
    {
        mResourcePtr = &resource;
        ++mResourcePtr->count;
    }

    Handle(const Handle& other) noexcept
    {
        mResourcePtr = other.mResourcePtr;
        if (mResourcePtr) ++mResourcePtr->count;
    }

    Handle(Handle&& other) noexcept
    {
        mResourcePtr = other.mResourcePtr;
        other.mResourcePtr = nullptr;
    }

    Handle& operator=(const Handle& other) noexcept
    {
        if (mResourcePtr) --mResourcePtr->count;
        mResourcePtr = other.mResourcePtr;
        if (mResourcePtr) ++mResourcePtr->count;
        return *this;
    }

    Handle& operator=(Handle&& other) noexcept
    {
        if (mResourcePtr) --mResourcePtr->count;
        mResourcePtr = other.mResourcePtr;
        other.mResourcePtr = nullptr;
        return *this;
    }

    ~Handle() noexcept
    {
        if (mResourcePtr) --mResourcePtr->count;
    }

    //--------------------------------------------------------//

    bool good() const noexcept
    {
        assert(mResourcePtr);
        return mResourcePtr->value != nullptr;
    }

    const Key& key() const noexcept
    {
        assert(mResourcePtr);
        return *mResourcePtr->key;
    }

    const Type* operator->() const noexcept
    {
        assert(mResourcePtr && mResourcePtr->value);
        return mResourcePtr->value.get();
    }

    const Type& operator*() const noexcept
    {
        assert(mResourcePtr && mResourcePtr->value);
        return *mResourcePtr->value;
    }

    const Type& value() const noexcept
    {
        assert(mResourcePtr && mResourcePtr->value);
        return *mResourcePtr->value;
    }

    const std::string& error() const noexcept
    {
        assert(mResourcePtr && !mResourcePtr->value);
        return mResourcePtr->error;
    }

    //--------------------------------------------------------//

    bool operator==(const Handle& other) const noexcept { return mResourcePtr == other.mResourcePtr; }

private: //===================================================//

    Resource<Key, Type>* mResourcePtr;
};

//============================================================================//

} // namespace sq
