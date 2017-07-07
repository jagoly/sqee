#pragma once

#include <memory>
#include <string>

#include <sqee/setup.hpp>
#include <sqee/assert.hpp>

namespace sq {

//============================================================================//

// Forward Declarations /////
template <class Type> class Handle;
template <class Type> class ResourceCache;

//============================================================================//

/// For internal use within a @ref ResourceCache.
/// @tparam Type: type of the resource

template <class Type>
class Resource final : NonCopyable
{
protected: //=================================================//

    friend class Handle<Type>;
    friend class ResourceCache<Type>;

    //--------------------------------------------------------//

    void decrement() { if (--count == 0u) uptr.reset(); }
    bool loaded() const { return uptr != nullptr; }

    //--------------------------------------------------------//

    std::unique_ptr<Type> uptr;
    uint count = 0u;

    std::string path;
};

//============================================================================//

/// Provides reference counted access to a @ref Resource.
/// @tparam Type type of the resource

template <class Type>
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

    /// Get the unique path of the resource.
    const std::string& get_path() const { return mResPtr ? mResPtr->path : ""; }

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

    friend class ResourceCache<Type>;

    //--------------------------------------------------------//

    Handle(Resource<Type>& resource)
    {
        mResPtr = &resource;
        ++mResPtr->count;
    }

    //--------------------------------------------------------//

    Resource<Type>* mResPtr;
};

//============================================================================//

} // namespace sq
