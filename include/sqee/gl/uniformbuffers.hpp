#pragma once
#include "forward.hpp"

#include <unordered_map>

namespace sq {

class Uniformbuffer : NonCopyable {
public:
    ~Uniformbuffer();

    void reserve(const string& _name, uint _size);
    void create();
    void bind(GLuint _index);

    void update(const string& _name, const void* _data);
    void update(const string& _name, const void* _data, uint _offs, uint _size);
    void update(uint _offs, uint _size, const void* _data);

    uint get_size();

    GLuint ubo = 0;

private:
    struct Item {
        Item(uint _offs, uint _size)
            : offs(_offs), size(_size) {}
        const uint offs, size;
    };
    std::unordered_map<string, Item> itemMap;
    uint crntSize = 0;
};

}
