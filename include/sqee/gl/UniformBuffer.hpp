#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

namespace sq {

/// A class for OpenGL uniform buffer objects
class UniformBuffer final : NonCopyable {
public:
    UniformBuffer();
    ~UniformBuffer();

    /// Add and reserve space for a uniform of a specified size
    void reserve(const string& _name, uint _size);

    /// Allocate the buffer. Call after reserve().
    void allocate_storage();

    /// Update a single uniform with a pointer
    void update(const string& _name, const void* _data);

    /// Update part of the buffer relative to a uniform
    void update(const string& _name, uint _offset, uint _size, const void* _data);

    /// Update an arbitary section of the buffer
    void update(uint _offset, uint _size, const void* _data);

    /// Check if the buffer has been allocated yet
    bool check_allocated() const { return allocated; }

    /// Bind the buffer to the specified index
    void bind(GLuint _index) const;

    /// Bind a range of the buffer to the specified index
    void bind(GLuint _index, uint _offset, uint _size) const;

    /// The OpenGL handle
    GLuint ubo = 0u;

private:
    bool allocated = false;
    struct Item { const uint offset, size; };
    std::unordered_map<string, Item> itemMap;
    uint currentSize = 0u;
};

}
