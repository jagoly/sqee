#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/DynamicBuffer.hpp>

using namespace sq;

DynamicBuffer::DynamicBuffer(GLenum _target) : target(_target) {
    gl::CreateBuffers(1, &buf);
}

DynamicBuffer::~DynamicBuffer() {
    gl::DeleteBuffers(1, &buf);
}

void DynamicBuffer::allocate_constant(uint _size, const void* _data) {
    gl::NamedBufferStorage(buf, _size, _data, gl::NONE);
}

void DynamicBuffer::allocate_editable(uint _size, const void *_data) {
    gl::NamedBufferStorage(buf, _size, _data, gl::DYNAMIC_STORAGE_BIT);
}

void DynamicBuffer::update(uint _offs, uint _size, const void* _data) {
    gl::NamedBufferSubData(buf, _offs, _size, _data);
}

void DynamicBuffer::bind() const {
    gl::BindBuffer(target, buf);
}

uint DynamicBuffer::get_size() const {
    int bufferSize;
    gl::GetNamedBufferParameteriv(buf, gl::BUFFER_SIZE, &bufferSize);
    return bufferSize;
}
