#pragma once

#include <sqee/setup.hpp>
#include <sqee/helpers.hpp>

namespace sq {

//============================================================================//

class Context; // Forward Declaration

//============================================================================//

/// OpenGL Uniform Buffer Object.
class UniformBuffer final : public MoveOnly
{
public: //====================================================//

    UniformBuffer(); ///< Constructor

    UniformBuffer(UniformBuffer&& other); ///< Move Constructor

    UniformBuffer& operator=(UniformBuffer&& other); ///< Move Assignment

    ~UniformBuffer(); ///< Destructor

    //--------------------------------------------------------//

    /// Allocate buffer storage.
    void create_and_allocate(uint size);

    /// Update a section of the buffer from a pointer.
    void update(uint offset, uint size, const void* data);

    //--------------------------------------------------------//

    /// Update a section of the buffer with a value.
    template <class T>
    void update(uint offset, const T& data)
    {
        update(offset, sizeof(T), &data);
    }

    /// Update the entire buffer with a set of values.
    template <class... Ts>
    void update_complete(const Ts&... args)
    {
        #ifndef SQEE_MSVC
        update(0u, Structure(args...));
        #else
        update(0u, Structure<std::remove_reference_t<decltype(args)>...>{args...});
        #endif
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
