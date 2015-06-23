#pragma once
#include <sqee/forward.hpp>

namespace sq {

class UniformBuffer : NonCopyable {
public:
    ~UniformBuffer();

    void reserve(const string& _name, uint _size);
    void create(); void bind(GLuint _index);

    void update(const string& _name, const void* _data);
    void update(const string& _name, const void* _data, uint _offs, uint _size);
    void update(uint _offs, uint _size, const void* _data);

    uint get_size();

    GLuint ubo = 0u;

private:
    struct Item {
        Item(uint _offs, uint _size) : offs(_offs), size(_size) {}
        const uint offs, size;
    };
    unordered_map<string, Item> itemMap;
    uint crntSize = 0u;
};

}
