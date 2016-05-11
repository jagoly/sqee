#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/UniformBuffer.hpp>

using namespace sq;

UniformBuffer::~UniformBuffer() {
    if (ubo != 0u) gl::DeleteBuffers(1, &ubo);
}

void UniformBuffer::delete_object() {
    if (ubo != 0u) gl::DeleteBuffers(1, &ubo);
    itemMap.clear(); currentSize = 0u;
}

void UniformBuffer::reserve(const string& _name, uint _size) {
    itemMap.emplace(_name, Item{currentSize, _size*4});
    currentSize += _size*4;
}

void UniformBuffer::create_and_allocate() {
    if (ubo != 0u) gl::DeleteBuffers(1, &ubo); gl::CreateBuffers(1, &ubo);
    gl::NamedBufferStorage(ubo, currentSize, nullptr, gl::DYNAMIC_STORAGE_BIT);
}

void UniformBuffer::update(const string& _name, const void* _data) {
    const Item& item = itemMap.at(_name);
    gl::NamedBufferSubData(ubo, item.offset, item.size, _data);
}

void UniformBuffer::update(const string& _name, uint _offset, uint _size, const void* _data) {
    const Item& item = itemMap.at(_name);
    gl::NamedBufferSubData(ubo, item.offset + _offset*4, _size*4, _data);
}

void UniformBuffer::update(uint _offset, uint _size, const void* _data) {
    gl::NamedBufferSubData(ubo, _offset*4, _size*4, _data);
}

void UniformBuffer::bind(GLuint _index) const {
    gl::BindBufferBase(gl::UNIFORM_BUFFER, _index, ubo);
}

void UniformBuffer::bind(GLuint _index, uint _offset, uint _size) const {
    gl::BindBufferRange(gl::UNIFORM_BUFFER, _index, ubo, _offset*4, _size*4);
}
