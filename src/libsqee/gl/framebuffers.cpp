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

    for (int i = 0; i < _bufCount; i++) {
        drawBuffers.push_back(_drawBuffers[i]);
        uint bufId = attachMap[_drawBuffers[i]];
        colourTextures[bufId] = sq::Texture::Ptr(new sq::Texture(gl::TEXTURE_2D, gl::RGBA));
        colourTextures[bufId]->bind();
        colourTextures[bufId]->set_params(2, MIN_MAG_FILTERS, BOTH_NEAREST);
        colourTextures[bufId]->set_params(2, S_T_WRAP, BOTH_CLAMP_TO_EDGE);
    }

    hasDepth = _depth;

    return false;
}

bool Framebuffer::resize(glm::uvec2 _size, glm::uvec2 _margins) {
    gl::BindFramebuffer(gl::FRAMEBUFFER, framebuffer);
    margins = _margins;
    size = _size + margins + margins;

    for (GLenum i : drawBuffers) {
        uint bufId = attachMap[i];
        colourTextures[bufId] = tex2D_load_blank(size, gl::RGBA16F);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, i,
                                 gl::TEXTURE_2D, colourTextures[bufId]->get(), 0);
    }

    if (hasDepth) {
        depthTexture = texDepth_load_blank(size, gl::DEPTH_COMPONENT32F);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT,
                                 gl::TEXTURE_2D, depthTexture->get(), 0);
    }

    if (gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO Error" << std::endl;
        return true;
    }

    return false;
}

Texture::Ptr Framebuffer::get(int _id) {
    if (_id == -1) {
        if (hasDepth) {
            return depthTexture;
        } else {
            std::cout << "ERROR: Framebuffer has no Depth Texture" << std::endl;
            return nullptr;
        }
    }
    Texture::Ptr ret = colourTextures[_id];
    if (ret == nullptr) std::cout << "ERROR: Framebuffer has no Texture " << _id << std::endl;
    return ret;
}

void Framebuffer::use() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, framebuffer);
    gl::DrawBuffers(drawBuffers.size(), drawBuffers.data());
}

void Framebuffer::useVP(bool _cutMargins) {
    if (!_cutMargins) gl::Viewport(0, 0, size.x, size.y);
    else              gl::Viewport(-margins.x, -margins.y, size.x, size.y);
}
