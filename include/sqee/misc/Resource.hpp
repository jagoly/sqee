#pragma once

#include <memory>

#include <sqee/setup.hpp>
#include <sqee/assert.hpp>

namespace sq {

//============================================================================//

template <class Type>
struct Resource
{
    void decrement() { if (--count == 0u) uptr.reset(); }
    bool loaded() const { return uptr != nullptr; }

    std::unique_ptr<Type> uptr;
    uint32_t count = 0u;

    // 4 bytes padding, use for something?
};

//============================================================================//

template <class Type>
struct Handle
{
    using ResourceT = Resource<Type>;

    //========================================================//

    Handle() : mResPtr(nullptr) {}

    ~Handle() { if (mResPtr) mResPtr->decrement(); }

    Handle(ResourceT& resource) { mResPtr = &resource; ++mResPtr->count; }

    Handle(const Handle& other) { if ((mResPtr = other.mResPtr)) ++mResPtr->count; }
    Handle& operator=(Handle other) { std::swap(mResPtr, other.mResPtr); return *this; }

    //========================================================//

    void set_null() { this->~Handle(); mResPtr = nullptr; }
    bool check() const { return mResPtr != nullptr; }

    //========================================================//

    bool operator==(const Handle& other) const { return mResPtr == other.mResPtr; }
    bool operator!=(const Handle& other) const { return mResPtr != other.mResPtr; }
    bool operator<(const Handle& other) const { return mResPtr < other.mResPtr; }

    //========================================================//

    const Type* operator->() const
    {
        SQASSERT(mResPtr != nullptr, "");
        SQASSERT(mResPtr->loaded(), "");
        return mResPtr->uptr.get();
    }

    const Type& get() const
    {
        SQASSERT(mResPtr != nullptr, "");
        SQASSERT(mResPtr->loaded(), "");
        return *mResPtr->uptr;
    }

    //========================================================//

    private: Resource<Type>* mResPtr;
};

} // namespace sq
