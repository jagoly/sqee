#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/render/Volume.hpp>

using namespace sq;

Volume::Volume(const float* _verts, const uchar* _elems, uchar _vCount, uchar _eCount)

    : vbo(gl::ARRAY_BUFFER), ebo(gl::ELEMENT_ARRAY_BUFFER),
      vertexCount(_vCount), elementCount(_eCount) {

    vbo.allocate_constant(_vCount * 12u, _verts);
    ebo.allocate_constant(_eCount, _elems);

    vao.add_attribute(vbo, 0u, 0u, 12u, 3u, gl::FLOAT, false);
    vao.set_element_buffer(ebo);
}

void Volume::bind_and_draw() const {
    gl::BindVertexArray(vao.get_handle());
    gl::DrawElements(gl::TRIANGLES, elementCount, gl::UNSIGNED_BYTE, nullptr);
}
