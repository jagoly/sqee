#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/DynamicBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>

using namespace sq;

VertexArray::VertexArray() {
    gl::CreateVertexArrays(1, &vao);
}

VertexArray::~VertexArray() {
    gl::DeleteVertexArrays(1, &vao);
}

void VertexArray::add_attribute(const FixedBuffer& _buffer, uint _index, uint _offs,
                                uint _stride, uint _size, GLenum _type, bool _normalize) const {
    gl::VertexArrayVertexBuffer(vao, _index, _buffer.get_handle(), _offs, _stride);
    gl::VertexArrayAttribFormat(vao, _index, _size, _type, _normalize, 0u);
    gl::EnableVertexArrayAttrib(vao, _index);
}

void VertexArray::add_attribute(const DynamicBuffer& _buffer, uint _index, uint _offs,
                                uint _stride, uint _size, GLenum _type, bool _normalize) const {
    gl::VertexArrayVertexBuffer(vao, _index, _buffer.get_handle(), _offs, _stride);
    gl::VertexArrayAttribFormat(vao, _index, _size, _type, _normalize, 0u);
    gl::EnableVertexArrayAttrib(vao, _index);
}


void VertexArray::add_attribute_I(const FixedBuffer& _buffer, uint _index, uint _offs,
                                  uint _stride, uint _size, GLenum _type) const {
    gl::VertexArrayVertexBuffer(vao, _index, _buffer.get_handle(), _offs, _stride);
    gl::VertexArrayAttribIFormat(vao, _index, _size, _type, 0u);
    gl::EnableVertexArrayAttrib(vao, _index);
}

void VertexArray::add_attribute_I(const DynamicBuffer& _buffer, uint _index, uint _offs,
                                  uint _stride, uint _size, GLenum _type) const {
    gl::VertexArrayVertexBuffer(vao, _index, _buffer.get_handle(), _offs, _stride);
    gl::VertexArrayAttribIFormat(vao, _index, _size, _type, 0u);
    gl::EnableVertexArrayAttrib(vao, _index);
}

void VertexArray::set_element_buffer(const sq::FixedBuffer& _buffer) const {
    gl::VertexArrayElementBuffer(vao, _buffer.get_handle());
}

void VertexArray::set_element_buffer(const sq::DynamicBuffer& _buffer) const {
    gl::VertexArrayElementBuffer(vao, _buffer.get_handle());
}

void VertexArray::bind() const {
    gl::BindVertexArray(vao);
}
