#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/FrameBuffer.hpp>

using namespace sq;

FrameBuffer::FrameBuffer() {
    gl::CreateFramebuffers(1, &fbo);
}

FrameBuffer::~FrameBuffer() {
    gl::DeleteFramebuffers(1, &fbo);
}


void FrameBuffer::attach(GLenum _attach, const Texture2D& _tex) const {
    gl::NamedFramebufferTexture(fbo, _attach, _tex.tex, 0);
}

void FrameBuffer::attach(GLenum _attach, const TextureCube& _tex, uint _face) const {
    gl::NamedFramebufferTextureLayer(fbo, _attach, _tex.tex, 0, _face);
}

void FrameBuffer::attach(GLenum _attach, const Texture2DArray& _tex, uint _index) const {
    gl::NamedFramebufferTextureLayer(fbo, _attach, _tex.tex, 0, _index);
}

void FrameBuffer::attach(GLenum _attach, const TextureCubeArray& _tex, uint _index, uint _face) const {
    gl::NamedFramebufferTextureLayer(fbo, _attach, _tex.tex, 0, _index * 6 + _face);
}


void FrameBuffer::draw_buffers(std::initializer_list<GLenum> _drawBufs) const {
    gl::NamedFramebufferDrawBuffers(fbo, _drawBufs.size(), _drawBufs.begin());
}

void FrameBuffer::bind() const {
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
}

void FrameBuffer::bind_read_only() const {
    gl::BindFramebuffer(gl::READ_FRAMEBUFFER, fbo);
}

void FrameBuffer::bind_draw_only() const {
    gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo);
}


void FrameBuffer::blit(const FrameBuffer& _other, Vec2U _sizeA, Vec2U _sizeB,
                       GLenum _mask, GLenum _filter) const {
    gl::BlitNamedFramebuffer(fbo, _other.fbo, 0, 0, _sizeA.x, _sizeA.y,
                             0, 0, _sizeB.x, _sizeB.y, _mask, _filter);
}
