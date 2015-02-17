#include "app/logging.hpp"
#include "gl/gl_ext_3_3.hpp"
#include "gl/maths.hpp"

#include "gl/framebuffers.hpp"

using namespace sq;

Framebuffer::Framebuffer(const std::vector<GLenum>& _drawBuffers,
                         const std::vector<GLenum>& _cFormats,
                         const std::vector<GLenum>& _icFormats,
                         GLenum _dsFormat, GLenum _idsFormat)
    : drawBuffers(_drawBuffers) {

    for (GLenum i : drawBuffers) {
        uint ind = i - gl::COLOR_ATTACHMENT0;
        cTexVec[ind].create(gl::TEXTURE_2D, _cFormats[ind], _icFormats[ind], Texture::Preset::N_C);
    }

    if (_dsFormat != gl::NONE) {
        hasDepthStencil = true;
        dsTex.create(gl::TEXTURE_2D, _dsFormat, _idsFormat, Texture::Preset::N_C);
    }

    gl::GenFramebuffers(1, &fbo);
}

Framebuffer::Framebuffer(const std::vector<GLenum>& _drawBuffers,
                         const std::vector<GLenum>& _cFormats,
                         const std::vector<GLenum>& _icFormats)
    : Framebuffer(_drawBuffers, _cFormats, _icFormats, gl::NONE, gl::NONE) {}

Framebuffer::Framebuffer(GLenum _dsFormat, GLenum _idsFormat)
    : Framebuffer({}, {}, {}, _dsFormat, _idsFormat) {}


void Framebuffer::resize(glm::uvec2 _size) {
    size = _size;

    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);

    for (GLenum i : drawBuffers) {
        Texture& texture = cTexVec[i - gl::COLOR_ATTACHMENT0];
        texture.resize({size.x, size.y, 1});
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, i, gl::TEXTURE_2D, texture.tex, 0);
    }

    if (hasDepthStencil) {
        dsTex.resize({size.x, size.y, 1});
        if (dsTex.format == gl::DEPTH_STENCIL)
            gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::STENCIL_ATTACHMENT,
                                     gl::TEXTURE_2D, dsTex.tex, 0);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT,
                                 gl::TEXTURE_2D, dsTex.tex, 0);
    }

    if (gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE)
        log_error("FBO resize failed");
}

void Framebuffer::resize(uint _size) {
    resize({_size, _size});
}

void Framebuffer::use() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    gl::DrawBuffers(drawBuffers.size(), drawBuffers.data());
}

void Framebuffer::useVP() {
    gl::Viewport(0, 0, size.x, size.y);
}
