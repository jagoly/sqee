#pragma once
#include <sqee/forward.hpp>
#include <unordered_map>

namespace sq {

/// A class for an OpenGL uniform buffer object
class UniformBuffer : NonCopyable {
public:
    ~UniformBuffer();

    /// Add and reserve space for a uniform of a specified size
    void reserve(const string& _name, uint _size);

    /// Create the buffer. Must be called after reserve().
    void create();

    /// Bind the UBO the specified index
    void bind(GLuint _index);

    /// Update a single uniform with a pointer
    void update(const string& _name, const void* _data);

    /// Update a portion of a single uniform with a pointer
    void update(const string& _name, const void* _data, uint _offs, uint _size);

    /// Update an arbitary section of a the buffer with a pointer
    void update(uint _offs, uint _size, const void* _data);

    /// Get the current size a the unif
    uint get_size();

    /// The OpenGL handle
    GLuint ubo = 0u;

protected:
    struct Item {
        Item(uint _offs, uint _size) : offs(_offs), size(_size) {}
        const uint offs, size;
    };
    std::unordered_map<string, Item> itemMap;
    uint crntSize = 0u;
};

}
