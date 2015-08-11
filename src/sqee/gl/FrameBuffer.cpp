#include <sqee/redist/gl_ext_4_1.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/FrameBuffer.hpp>

using namespace sq;

FrameBuffer::FrameBuffer() {
    gl::GenFramebuffers(1, &fbo);
}

FrameBuffer::~FrameBuffer() {
    gl::DeleteFramebuffers(1, &fbo);
}

void FrameBuffer::attach(GLenum _attach, TextureMut2D& _tex) {
    bind(gl::FRAMEBUFFER); _tex.bind(); add_buf(_attach);
    gl::FramebufferTexture(gl::FRAMEBUFFER, _attach, _tex.tex, 0);
}

void FrameBuffer::attach(GLenum _attach, TextureMutCube& _tex, uint _face) {
    bind(gl::FRAMEBUFFER); _tex.bind(); add_buf(_attach);
    GLenum target = gl::TEXTURE_CUBE_MAP_POSITIVE_X + _face;
    gl::FramebufferTexture2D(gl::FRAMEBUFFER, _attach, target, _tex.tex, 0);
}

void FrameBuffer::attach(GLenum _attach, TextureMut2DArray& _tex, uint _index) {
    bind(gl::FRAMEBUFFER); _tex.bind(); add_buf(_attach);
    gl::FramebufferTextureLayer(gl::FRAMEBUFFER, _attach, _tex.tex, 0, _index);
}

void FrameBuffer::attach(GLenum _attach, TextureMutCubeArray& _tex, uint _face, uint _index) {
    bind(gl::FRAMEBUFFER); _tex.bind(); add_buf(_attach);
    gl::FramebufferTextureLayer(gl::FRAMEBUFFER, _attach, _tex.tex, 0, _index * 6 + _face);
}

void FrameBuffer::bind(GLenum _target) {
    gl::BindFramebuffer(_target, fbo);
}

void FrameBuffer::draw_bufs() {
    gl::DrawBuffers(drawBufs.size(), drawBufs.data());
}

void FrameBuffer::use() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    gl::DrawBuffers(drawBufs.size(), drawBufs.data());
}

void FrameBuffer::add_buf(GLenum _attach) {
    if (_attach != gl::DEPTH_ATTACHMENT && _attach != gl::STENCIL_ATTACHMENT &&
        _attach != gl::DEPTH_STENCIL_ATTACHMENT && !bufSet.count(_attach))
        drawBufs.emplace_back(_attach), bufSet.emplace(_attach);
}
