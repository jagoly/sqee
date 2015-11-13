#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/FixedBuffer.hpp>

using namespace sq;

FixedBuffer::FixedBuffer(GLenum _target) : target(_target) {
    gl::CreateBuffers(1, &buf);
}

FixedBuffer::~FixedBuffer() {
    gl::DeleteBuffers(1, &buf);
}

void FixedBuffer::allocate_constant(uint _size, const void* _data) const {
    gl::NamedBufferStorage(buf, _size, _data, gl::NONE);
}

void FixedBuffer::allocate_editable(uint _size, const void* _data) const {
    gl::NamedBufferStorage(buf, _size, _data, gl::DYNAMIC_STORAGE_BIT);
}

void FixedBuffer::update(uint _offset, uint _size, const void* _data) const {
    gl::NamedBufferSubData(buf, _offset, _size, _data);
}

void FixedBuffer::bind() const {
    gl::BindBuffer(target, buf);
}
