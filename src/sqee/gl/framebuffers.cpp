#include <unordered_map>

#include <gl/framebuffers.hpp>

using namespace sq;

void Framebuffer::create(const vector<GLenum>& _drawBuffers,
                         const vector<GLenum>& _cFormats,
                         const vector<GLenum>& _icFormats,
                         GLenum _dsFormat, GLenum _idsFormat) {

    drawBuffers = _drawBuffers;
    for (GLenum i : drawBuffers) {
        uint ind = i - gl::COLOR_ATTACHMENT0;
        cTexVec[ind].create(gl::TEXTURE_2D, _cFormats[ind], _icFormats[ind], Texture::Preset::N_C);
    }

    if (_dsFormat != gl::NONE) {
        hasDepthStencil = true;
        dsTex.create(gl::TEXTURE_2D, _dsFormat, _idsFormat, Texture::Preset::N_C);
    }

    gl::DeleteFramebuffers(1, &fbo);
    gl::GenFramebuffers(1, &fbo);
}

void Framebuffer::create(const vector<GLenum>& _drawBuffers,
                         const vector<GLenum>& _cFormats,
                         const vector<GLenum>& _icFormats) {
    create(_drawBuffers, _cFormats, _icFormats, gl::NONE, gl::NONE);
}

void Framebuffer::create(GLenum _dsFormat, GLenum _idsFormat) {
    create({}, {}, {}, _dsFormat, _idsFormat);
}


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
        cout << "ERROR: FBO resize failed" << endl;
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
