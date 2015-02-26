#include "app/logging.hpp"
#include "gl/gl_ext_3_3.hpp"

#include "gl/framebuffers.hpp"

using namespace sq;

Framebuffer::Framebuffer() {
    gl::GenFramebuffers(1, &fbo);
}

void Framebuffer::create_tex(int _id, GLenum _format, GLenum _iFormat, Texture::Preset _preset) {
    if (_id < 0) {
        dsTex.reset(new Texture());
        dsTex->create(gl::TEXTURE_2D, _format, _iFormat, _preset);
    } else {
        cTexArr[_id].reset(new Texture());
        cTexArr[_id]->create(gl::TEXTURE_2D, _format, _iFormat, _preset);
    }
}

void Framebuffer::resize(glm::uvec2 _size) {
    size = _size;

    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    drawBuffers.clear();

    for (uint i = 0; i < 8; i++) {
        auto& tex = cTexArr[i];
        if (tex.get() != nullptr) {
            tex->resize({size.x, size.y, 1});
            gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0+i,
                                     gl::TEXTURE_2D, tex->tex, 0);
            drawBuffers.emplace_back(gl::COLOR_ATTACHMENT0+i);
        }
    }

    if (dsTex.get() != nullptr) {
        dsTex->resize({size.x, size.y, 1});
        if (dsTex->format == gl::DEPTH_STENCIL)
            gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::STENCIL_ATTACHMENT,
                                     gl::TEXTURE_2D, dsTex->tex, 0);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT,
                                 gl::TEXTURE_2D, dsTex->tex, 0);
    }

    if (gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE)
        log_error("FBO resize failed");
}

void Framebuffer::use() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    gl::DrawBuffers(drawBuffers.size(), drawBuffers.data());
}

void Framebuffer::useVP() {
    gl::Viewport(0, 0, size.x, size.y);
}
