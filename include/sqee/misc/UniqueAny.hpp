/*#pragma once

#include <sqee/setup.hpp>
#include <sqee/assert.hpp>

//============================================================================//

namespace sq {

class UniqueAny : public sq::MoveOnly
{
public: //====================================================//

    UniqueAny() noexcept
    {
        mDataStore = nullptr;
        mTypeInfo = &typeid(void);
        mDestructor = nullptr;
    }

    ~UniqueAny()
    {
        if (has_value() == true)
            mDestructor(mDataStore);
    }

    //--------------------------------------------------------//

    UniqueAny(UniqueAny&& other) noexcept
    {
        mDataStore = other.mDataStore;
        mTypeInfo = other.mTypeInfo;
        mDestructor = other.mDestructor;

        other.mDataStore = nullptr;
        other.mTypeInfo = &typeid(void);
        other.mDestructor = nullptr;
    }

    UniqueAny& operator=(UniqueAny&& other)
    {
        if (has_value() == true)
            mDestructor(mDataStore);

        mDataStore = other.mDataStore;
        mTypeInfo = other.mTypeInfo;
        mDestructor = other.mDestructor;

        other.mDataStore = nullptr;
        other.mTypeInfo = &typeid(void);
        other.mDestructor = nullptr;

        return *this;
    }

    //--------------------------------------------------------//

    template <class T, class... Args>
    void emplace(Args&&... args)
    {
        SQASSERT(has_value() == false, "");

        mDataStore = new T(std::forward<Args>(args)...);
        mTypeInfo = &typeid(T);
        mDestructor = &impl_destroy<T>;
    }

    template <class T, class... Args>
    static UniqueAny make(Args&&... args)
    {
        UniqueAny result;
        result.emplace<T>(std::forward<Args>(args)...);
        return result;
    }

    //--------------------------------------------------------//

    template <class T> T& get()
    {
        SQASSERT(has_value() == true, "");
        SQASSERT(*mTypeInfo == typeid(T), "");

        return *static_cast<T*>(mDataStore);
    }

    template <class T> const T& get() const
    {
        SQASSERT(has_value() == true, "");
        SQASSERT(*mTypeInfo == typeid(T), "");

        return *static_cast<T*>(mDataStore);
    }

    //--------------------------------------------------------//

    void reset()
    {
        if (has_value() == true)
        {
            mDestructor(mDataStore);

            mDataStore = nullptr;
            mTypeInfo = &typeid(void);
            mDestructor = nullptr;
        }
    }

    //--------------------------------------------------------//

    bool has_value() const noexcept
    {
        return mDataStore != nullptr;
    }

    const std::type_info& type() const noexcept
    {
        return *mTypeInfo;
    }

    //--------------------------------------------------------//

    bool operator==(void* other) const noexcept
    {
        return mDataStore == other;
    }

    bool operator!=(void* other) const noexcept
    {
        return mDataStore != other;
    }

private: //===================================================//

    template <class T>
    static void impl_destroy(void* ptr)
    {
        delete static_cast<T*>(ptr);
    }

    void* mDataStore;
    const std::type_info* mTypeInfo;
    void (*mDestructor)(void*);
};

} // namespace sq*/
