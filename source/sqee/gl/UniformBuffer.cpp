#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/UniformBuffer.hpp>

using namespace sq;

UniformBuffer::UniformBuffer() {
    gl::CreateBuffers(1, &ubo);
}

UniformBuffer::~UniformBuffer() {
    gl::DeleteBuffers(1, &ubo);
}

void UniformBuffer::reserve(const string& _name, uint _size) {
    itemMap.emplace(_name, Item{currentSize, _size});
    currentSize += _size;
}

void UniformBuffer::allocate_storage() const {
    gl::NamedBufferStorage(ubo, currentSize*4, nullptr, gl::DYNAMIC_STORAGE_BIT);
}

void UniformBuffer::bind(GLuint _index) const {
    gl::BindBufferBase(gl::UNIFORM_BUFFER, _index, ubo);
}

void UniformBuffer::update(const std::string& _name, const void* _data) const {
    const Item& item = itemMap.at(_name);
    gl::NamedBufferSubData(ubo, item.offs*4, item.size*4, _data);
}

void UniformBuffer::update(const std::string& _name, uint _offs, uint _size, const void* _data) const {
    const Item& item = itemMap.at(_name);
    gl::NamedBufferSubData(ubo, (item.offs+_offs)*4, _size*4, _data);
}

void UniformBuffer::update(uint _offs, uint _size, const void* _data) const {
    gl::NamedBufferSubData(ubo, _offs*4, _size*4, _data);
}
