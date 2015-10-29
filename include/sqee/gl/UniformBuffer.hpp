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
    void allocate_storage() const;

    /// Bind the UBO to the specified index
    void bind(GLuint _index) const;

    /// Update a single uniform with a pointer
    void update(const string& _name, const void* _data) const;

    /// Update a portion of a single uniform with a pointer
    void update(const string& _name, uint _offs, uint _size, const void* _data) const;

    /// Update an arbitary section of the buffer with a pointer
    void update(uint _offs, uint _size, const void* _data) const;

    /// The OpenGL handle
    GLuint ubo = 0u;

private:
    struct Item { const uint offs, size; };
    std::unordered_map<string, Item> itemMap;
    uint currentSize = 0u;
};

}
