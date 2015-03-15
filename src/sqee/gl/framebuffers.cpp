#include "gl/gl_ext_3_3.hpp"
#include "gl/textures.hpp"
#include "gl/framebuffers.hpp"

using namespace sq;

Framebuffer::Framebuffer() {
    gl::GenFramebuffers(1, &fbo);
}

void Framebuffer::attach(GLenum _attach, Texture& _tex) {
    add_buf(_attach);
    gl::FramebufferTexture(gl::FRAMEBUFFER, _attach, _tex.tex, 0);
}

void Framebuffer::attach_layer(GLenum _attach, Texture& _tex, GLint _layer) {
    add_buf(_attach);
    gl::FramebufferTextureLayer(gl::FRAMEBUFFER, _attach, _tex.tex, 0, _layer);
}

void Framebuffer::bind() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
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
        _attach != gl::DEPTH_STENCIL_ATTACHMENT && !bufSet.count(_attach)) {
        drawBufs.emplace_back(_attach);
        bufSet.emplace(_attach);
    }
}
