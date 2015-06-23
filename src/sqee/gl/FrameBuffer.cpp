#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/FrameBuffer.hpp>

using namespace sq;

FrameBuffer::FrameBuffer() {
    gl::GenFramebuffers(1, &fbo);
}

FrameBuffer::~FrameBuffer() {
    gl::DeleteFramebuffers(1, &fbo);
}

void FrameBuffer::attach(GLenum _attach, Texture2D& _tex) {
    bind(gl::FRAMEBUFFER); _tex.bind(); add_buf(_attach);
    gl::FramebufferTexture(gl::FRAMEBUFFER, _attach, _tex.tex, 0);
}

void FrameBuffer::attach(GLenum _attach, TextureArray& _tex, GLint _layer) {
    bind(gl::FRAMEBUFFER); _tex.bind(); add_buf(_attach);
    gl::FramebufferTextureLayer(gl::FRAMEBUFFER, _attach, _tex.tex, 0, _layer);
}

void FrameBuffer::attach(GLenum _attach, TextureCube& _tex, GLint _layer) {
    bind(gl::FRAMEBUFFER); _tex.bind(); add_buf(_attach);
    gl::FramebufferTexture2D(gl::FRAMEBUFFER, _attach, 0x8515+_layer, _tex.tex, 0);
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
