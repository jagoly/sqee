// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

namespace sq {

//====== Forward Declarations ================================================//

class Context;

//============================================================================//

/// OpenGL Uniform Buffer Object.
class SQEE_API UniformBuffer final : private MoveOnly
{
public: //====================================================//

    UniformBuffer(); ///< Constructor

    UniformBuffer(UniformBuffer&& other) noexcept; ///< Move Constructor

    UniformBuffer& operator=(UniformBuffer&& other) noexcept; ///< Move Assignment

    ~UniformBuffer() noexcept; ///< Destructor

    //--------------------------------------------------------//

    /// Allocate buffer storage.
    void create_and_allocate(uint size);

    /// Update a section of the buffer from a pointer.
    void update(uint offset, uint size, const void* data);

    /// Update a section of the buffer with a value.
    template <class T>
    void update(uint offset, const T& data)
    {
        update(offset, sizeof(T), &data);
    }

    //--------------------------------------------------------//

    /// Get the OpenGL object handle.
    GLuint get_handle() const { return mHandle; }

private: //===================================================//

    Context& mContext;

    uint mBufferSize = 0u;

    GLuint mHandle = 0u;
};


//============================================================================//

} // namespace sq
