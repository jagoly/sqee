#include <iostream>
#include <array>
#include <unordered_map>

#include <gl/framebuffers.hpp>

using namespace sq;

std::unordered_map<GLenum, uint> attachMap = {
    {gl::COLOR_ATTACHMENT0,  0},  {gl::COLOR_ATTACHMENT1,  1},
    {gl::COLOR_ATTACHMENT2,  2},  {gl::COLOR_ATTACHMENT3,  3},
    {gl::COLOR_ATTACHMENT4,  4},  {gl::COLOR_ATTACHMENT5,  5},
    {gl::COLOR_ATTACHMENT6,  6},  {gl::COLOR_ATTACHMENT7,  7},
    {gl::COLOR_ATTACHMENT8,  8},  {gl::COLOR_ATTACHMENT9,  9},
    {gl::COLOR_ATTACHMENT10, 10}, {gl::COLOR_ATTACHMENT11, 11},
    {gl::COLOR_ATTACHMENT12, 12}, {gl::COLOR_ATTACHMENT13, 13},
    {gl::COLOR_ATTACHMENT14, 14}, {gl::COLOR_ATTACHMENT15, 15}
};


void Framebuffer::create(std::vector<GLenum> _drawBuffers, std::vector<GLenum> _cFormats,
                         std::vector<GLenum> _iCFormats) {
    if (!fbo) gl::GenFramebuffers(1, &fbo);

    cTexVec.resize(_drawBuffers.size());

    for (uint i = 0; i < _drawBuffers.size(); i++) {
        drawBuffers.push_back(_drawBuffers[i]);
        uint bufId = attachMap[_drawBuffers[i]];
        cTexVec[bufId].create(gl::TEXTURE_2D, _cFormats[i], _iCFormats[i],
                              glm::uvec3(), TexPreset::N_C);
    }
}

void Framebuffer::create(GLenum _dsFormat, GLenum _idsFormat) {
    if (!fbo) gl::GenFramebuffers(1, &fbo);

    dsTex.create(gl::TEXTURE_2D, _dsFormat, _idsFormat, glm::uvec3(), TexPreset::N_C);
}

void Framebuffer::create(std::vector<GLenum> _drawBuffers,
                         std::vector<GLenum> _cFormats, std::vector<GLenum> _icFormats,
                         GLenum _dsFormat, GLenum _idsFormat) {
    create(_drawBuffers, _cFormats, _icFormats);
    create(_dsFormat, _idsFormat);
}

void Framebuffer::resize(glm::uvec2 _size) {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    size = _size;

    for (GLenum i : drawBuffers) {
        uint bufId = attachMap[i];
        cTexVec[bufId].resize({size.x, size.y, 1});
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, i,
                                 gl::TEXTURE_2D, cTexVec[bufId].tex, 0);
    }

    if (dsTex.tex) {
        dsTex.resize({size.x, size.y, 1});
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT,
                                 gl::TEXTURE_2D, dsTex.tex, 0);
    }

    if (gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: FBO resize failed" << std::endl;
}


void Framebuffer::use() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    gl::DrawBuffers(drawBuffers.size(), drawBuffers.data());
}

void Framebuffer::useVP() {
    gl::Viewport(0, 0, size.x, size.y);
}
