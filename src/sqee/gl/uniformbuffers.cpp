#include "sqee/redist/gl_ext_3_3.hpp"
#include "sqee/gl/uniformbuffers.hpp"

using namespace sq;

Uniformbuffer::~Uniformbuffer() {
    gl::DeleteBuffers(1, &ubo);
}

void Uniformbuffer::reserve(const string& _name, uint _size) {
    itemMap.emplace(_name, Item(crntSize, _size));
    crntSize += _size;
}

void Uniformbuffer::create() {
    gl::GenBuffers(1, &ubo);
    gl::BindBuffer(gl::UNIFORM_BUFFER, ubo);
    gl::BufferData(gl::UNIFORM_BUFFER, crntSize * 4, nullptr, gl::DYNAMIC_DRAW);
}

void Uniformbuffer::bind(GLuint _index) {
    gl::BindBufferBase(gl::UNIFORM_BUFFER, _index, ubo);
}

void Uniformbuffer::update(const string& _name, const void* _data) {
    const Item& item = itemMap.at(_name);
    gl::BufferSubData(gl::UNIFORM_BUFFER, item.offs*4, item.size*4, _data);
}

void Uniformbuffer::update(const string& _name, const void* _data, uint _offs, uint _size) {
    const Item& item = itemMap.at(_name);
    gl::BufferSubData(gl::UNIFORM_BUFFER, (item.offs+_offs)*4, _size*4, _data);
}

void Uniformbuffer::update(uint _offs, uint _size, const void* _data) {
    gl::BufferSubData(gl::UNIFORM_BUFFER, _offs*4, _size*4, _data);
}

uint Uniformbuffer::get_size() {
    if (ubo != 0) return crntSize;
    return 0;
}
