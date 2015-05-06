#include "sqee/redist/gl_ext_3_3.hpp"
#include "sqee/gl/textures.hpp"
#include "sqee/gl/framebuffers.hpp"

using namespace sq;

Framebuffer::Framebuffer() {
    gl::GenFramebuffers(1, &fbo);
}

void Framebuffer::attach(GLenum _attach, Texture2D& _tex) {
    add_buf(_attach); _tex.bind(); bind();
    gl::FramebufferTexture(gl::FRAMEBUFFER, _attach, _tex.tex, 0);
}

void Framebuffer::attach(GLenum _attach, Texture2DArray& _tex, GLint _layer) {
    add_buf(_attach); _tex.bind(); bind();
    gl::FramebufferTextureLayer(gl::FRAMEBUFFER, _attach, _tex.tex, 0, _layer);
}

void Framebuffer::attach(GLenum _attach, TextureCube& _tex, GLint _layer) {
    add_buf(_attach); _tex.bind(); bind();
    gl::FramebufferTexture2D(gl::FRAMEBUFFER, _attach, gl::TEXTURE_CUBE_MAP_POSITIVE_X+_layer, _tex.tex, 0);
}

void Framebuffer::bind() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
}

void Framebuffer::draw_bufs() {
    gl::DrawBuffers(drawBufs.size(), drawBufs.data());
}

bool Framebuffer::check() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    return gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE;
}

void Framebuffer::use() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    gl::DrawBuffers(drawBufs.size(), drawBufs.data());
}

void Framebuffer::add_buf(GLenum _attach) {
    if (_attach != gl::DEPTH_ATTACHMENT && _attach != gl::STENCIL_ATTACHMENT &&
        _attach != gl::DEPTH_STENCIL_ATTACHMENT && !bufSet.count(_attach))
        drawBufs.emplace_back(_attach), bufSet.emplace(_attach);
}
