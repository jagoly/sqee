#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/FixedBuffer.hpp>

using namespace sq;

FixedBuffer::FixedBuffer(GLenum _target) : target(_target) {
    gl::CreateBuffers(1, &buf);
}

FixedBuffer::~FixedBuffer() {
    gl::DeleteBuffers(1, &buf);
}

void FixedBuffer::allocate_constant(uint _size, const void* _data) {
    gl::NamedBufferStorage(buf, _size, _data, gl::NONE);
}

void FixedBuffer::allocate_editable(uint _size, const void *_data) {
    gl::NamedBufferStorage(buf, _size, _data, gl::DYNAMIC_STORAGE_BIT);
}

void FixedBuffer::update(uint _offs, uint _size, const void* _data) {
    gl::NamedBufferSubData(buf, _offs, _size, _data);
}

void FixedBuffer::bind() const {
    gl::BindBuffer(target, buf);
}

uint FixedBuffer::get_size() const {
    int bufferSize;
    gl::GetNamedBufferParameteriv(buf, gl::BUFFER_SIZE, &bufferSize);
    return bufferSize;
}
