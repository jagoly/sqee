#include <gl/framebuffers.hpp>

#include <iostream>

using namespace sq;

std::map<const GLenum, const uint> attachMap = {
    {gl::COLOR_ATTACHMENT0,  0},  {gl::COLOR_ATTACHMENT1,  1},
    {gl::COLOR_ATTACHMENT2,  2},  {gl::COLOR_ATTACHMENT3,  3},
    {gl::COLOR_ATTACHMENT4,  4},  {gl::COLOR_ATTACHMENT5,  5},
    {gl::COLOR_ATTACHMENT6,  6},  {gl::COLOR_ATTACHMENT7,  7},
    {gl::COLOR_ATTACHMENT8,  8},  {gl::COLOR_ATTACHMENT9,  9},
    {gl::COLOR_ATTACHMENT10, 10}, {gl::COLOR_ATTACHMENT11, 11},
    {gl::COLOR_ATTACHMENT12, 12}, {gl::COLOR_ATTACHMENT13, 13},
    {gl::COLOR_ATTACHMENT14, 14}, {gl::COLOR_ATTACHMENT15, 15}
};

Framebuffer::Framebuffer() {
    colourTextures.resize(16);
}

bool Framebuffer::create(int _bufCount, const GLenum* _drawBuffers, bool _depth) {
    gl::GenFramebuffers(1, &framebuffer);

    cTexVec.resize(_bufCount);
    for (int i = 0; i < _bufCount; i++) {;
        colourTextures[attachMap[_drawBuffers[i]]] = &cTexVec[i];
        drawBuffers.push_back(_drawBuffers[i]);
    }

    for (Texture2D& tex : cTexVec) {
        tex.bind();
        tex.set_params(2, MIN_MAG_FILTERS, BOTH_NEAREST);
        tex.set_params(2, S_T_WRAP, BOTH_CLAMP_TO_EDGE);
    }

    hasDepth = _depth;

    return false;
}

bool Framebuffer::resize(glm::uvec2 _size) {
    gl::BindFramebuffer(gl::FRAMEBUFFER, framebuffer);

    for (int i = 0; i < cTexVec.size(); i++) {
        cTexVec[i].load_blank(_size, gl::RGBA16F);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, drawBuffers[i],
                                 gl::TEXTURE_2D, cTexVec[i].tex, 0);
    }

    if (hasDepth) {
        depthTexture.load_blank(_size, gl::DEPTH_COMPONENT32F);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT,
                                 gl::TEXTURE_2D, depthTexture.tex, 0);
    }

    if (gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO Error" << std::endl;
    }
}

void Framebuffer::use() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, framebuffer);
    gl::DrawBuffers(cTexVec.size(), drawBuffers.data());
}
