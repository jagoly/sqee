// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <sqee/gl/Types.hpp>

namespace sq {

//============================================================================//

/// OpenGL Generic Buffer Object.
class SQEE_API FixedBuffer final : private MoveOnly
{
public: //====================================================//

    FixedBuffer(); ///< Constructor.

    FixedBuffer(FixedBuffer&& other) noexcept; ///< Move Constructor.

    FixedBuffer& operator=(FixedBuffer&& other) noexcept; ///< Move Assignment.

    ~FixedBuffer() noexcept; ///< Destructor.

    //--------------------------------------------------------//

    /// Allocate the buffer with static data.
    void allocate_static(uint size, const void* data);

    /// Allocate the buffer with dynamic data.
    void allocate_dynamic(uint size);

    //--------------------------------------------------------//

    /// Update a section of the buffer with a pointer.
    void update(uint offset, uint size, const void* data);

    /// Update a section of the buffer with a value.
    template <class T>
    void update(uint offset, const T& data)
    {
        static_assert(std::is_pointer_v<T> == false, "");
        update(offset, sizeof(T), &data);
    }

    //--------------------------------------------------------//

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

private: //===================================================//

    GLuint mHandle = 0u;
};

//============================================================================//

} // namespace sq
