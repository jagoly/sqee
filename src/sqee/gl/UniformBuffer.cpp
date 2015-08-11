#include <sqee/redist/gl_ext_4_1.hpp>
#include <sqee/gl/UniformBuffer.hpp>

using namespace sq;

UniformBuffer::~UniformBuffer() {
    gl::DeleteBuffers(1, &ubo);
}

void UniformBuffer::reserve(const string& _name, uint _size) {
    itemMap.emplace(_name, Item(crntSize, _size));
    crntSize += _size;
}

void UniformBuffer::create() {
    gl::GenBuffers(1, &ubo);
    gl::BindBuffer(gl::UNIFORM_BUFFER, ubo);
    gl::BufferData(gl::UNIFORM_BUFFER, crntSize * 4, nullptr, gl::DYNAMIC_DRAW);
}

void UniformBuffer::bind(GLuint _index) {
    gl::BindBufferBase(gl::UNIFORM_BUFFER, _index, ubo);
}

void UniformBuffer::update(const string& _name, const void* _data) {
    const Item& item = itemMap.at(_name);
    gl::BufferSubData(gl::UNIFORM_BUFFER, item.offs*4, item.size*4, _data);
}

void UniformBuffer::update(const string& _name, const void* _data, uint _offs, uint _size) {
    const Item& item = itemMap.at(_name);
    gl::BufferSubData(gl::UNIFORM_BUFFER, (item.offs+_offs)*4, _size*4, _data);
}

void UniformBuffer::update(uint _offs, uint _size, const void* _data) {
    gl::BufferSubData(gl::UNIFORM_BUFFER, _offs*4, _size*4, _data);
}

uint UniformBuffer::get_size() {
    if (ubo == 0u) return 0u;
    else return crntSize;
}
