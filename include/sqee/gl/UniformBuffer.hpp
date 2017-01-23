#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

namespace sq {

//============================================================================//

class Context; // Forward Declaration

//============================================================================//

/// OpenGL Uniform Buffer Object
class UniformBuffer final : public MoveOnly
{
public:

    //========================================================//

    /// Constructor
    UniformBuffer();

    /// Move Constructor
    UniformBuffer(UniformBuffer&& other);

    /// Destructor
    ~UniformBuffer();

    //========================================================//

    /// Reserve space for a uniform of size
    void reserve(const string& name, uint size);

    /// Create object and allocate storage
    void create_and_allocate();

    //========================================================//

    /// Update a single uniform with a pointer
    void update(const string& name, const void* data);

    /// Update part of the buffer relative to a uniform
    void update(const string& name, uint offset, uint size, const void* data);

    /// Update an arbitary section of the buffer
    void update(uint offset, uint size, const void* data);

    /// Get the current size of the reserve list
    uint get_size() const { return mCurrentSize; }

    //========================================================//

    /// Get the OpenGL object handle
    GLuint get_handle() const { return mHandle; }

private:

    //========================================================//

    struct Item { const uint offset, size; };
    std::unordered_map<string, Item> mItemMap;
    uint mCurrentSize = 0u;

    GLuint mHandle = 0u;
    Context& mContext;
};

//============================================================================//

} // namespace sq
