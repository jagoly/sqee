#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

namespace sq {

/// A class for OpenGL uniform buffer objects
class UniformBuffer final : NonCopyable {
public:
    ~UniformBuffer();

    /// Constructor
    UniformBuffer() = default;

    /// Delete the GL object and clear layout
    void delete_object();

    /// Add and reserve space for a uniform of a specified size
    void reserve(const string& _name, uint _size);

    /// Create object and allocate storage. Call after reserve().
    void create_and_allocate();

    /// Update a single uniform with a pointer
    void update(const string& _name, const void* _data);

    /// Update part of the buffer relative to a uniform
    void update(const string& _name, uint _offset, uint _size, const void* _data);

    /// Update an arbitary section of the buffer
    void update(uint _offset, uint _size, const void* _data);

    /// Get the current size of the reserve list
    uint get_size() const { return currentSize; }

    /// Get the OpenGL handle
    const GLuint& get_handle() const { return ubo; }

private:
    /// The OpenGL handle
    GLuint ubo = 0u;

    struct Item { const uint offset, size; };
    std::unordered_map<string, Item> itemMap;
    uint currentSize = 0u;
};

}
