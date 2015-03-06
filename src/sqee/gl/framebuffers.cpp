#include <algorithm>

#include "app/logging.hpp"
#include "gl/gl_ext_3_3.hpp"

#include "gl/framebuffers.hpp"

using namespace sq;

//Framebuffer::Framebuffer() {
//    gl::GenFramebuffers(1, &fbo);
//}

//void Framebuffer::create_colour(uint _id, GLenum _format, GLenum _iFormat, Texture::Preset _preset) {
//    cTexArr[_id].reset(new Texture());
//    cTexArr[_id]->create(gl::TEXTURE_2D, _format, _iFormat, _preset);
//}

//void Framebuffer::create_depth(GLenum _format, GLenum _iFormat, Texture::Preset _preset) {
//    dsTex.reset(new Texture());
//    dsTex->create(gl::TEXTURE_2D, _format, _iFormat, _preset);
//}

//void Framebuffer::resize(glm::uvec2 _size) {
//    size = _size;

//    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
//    drawBuffers.clear();

//    for (uint i = 0; i < 8; i++) {
//        auto& tex = cTexArr[i];
//        if (tex.get() != nullptr) {
//            tex->resize({size.x, size.y, 1});
//            gl::FramebufferTexture(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0+i, tex->tex, 0);
//            drawBuffers.emplace_back(gl::COLOR_ATTACHMENT0+i);
//        }
//    }

//    if (dsTex.get() != nullptr) {
//        dsTex->resize({size.x, size.y, 1});
//        if (dsTex->format == gl::DEPTH_STENCIL)
//            gl::FramebufferTexture(gl::FRAMEBUFFER, gl::STENCIL_ATTACHMENT, dsTex->tex, 0);
//        gl::FramebufferTexture(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT, dsTex->tex, 0);
//    }

//    if (gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE)
//        log_error("FBO resize failed");
//}

//void Framebuffer::use() {
//    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
//    gl::DrawBuffers(drawBuffers.size(), drawBuffers.data());
//}

//void Framebuffer::useVP() {
//    gl::Viewport(0, 0, size.x, size.y);
//}


Framebuffer::Framebuffer() {
    gl::GenFramebuffers(1, &fbo);
}

void Framebuffer::attach_2d(GLenum _attach, Texture& _tex) {
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
