#pragma once

#include <gl/gl.hpp>

namespace sq {

class UniformBuffer : NonCopyable {
public:
    ~UniformBuffer();

    void reserve(const string& _name, uint _size);
    void create();
    void bind(GLuint _index);

    void update(const string& _name, const void* _data, uint _offs=0, uint _size=0);
    void update(uint _offs, uint _size, const void* _data);

    GLuint ubo = 0;

private:
    struct Item {
        Item(uint _offs, uint _size)
            : offs(_offs), size(_size) {}
        const uint offs, size;
    };
    unordered_map<string, Item> itemMap;
    uint crntSize = 0;
};

}
