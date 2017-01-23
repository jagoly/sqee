#include <sqee/assert.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/gl/FrameBuffer.hpp>

using namespace sq;

//============================================================================//

FrameBuffer::FrameBuffer()
{
    gl::CreateFramebuffers(1, &mHandle);
}

FrameBuffer::FrameBuffer(FrameBuffer&& other)
{
    Context::get().impl_reset_FrameBuffer(&other, this);
    mHandle = other.mHandle; other.mHandle = 0u;
}

FrameBuffer::~FrameBuffer()
{
    Context::get().impl_reset_FrameBuffer(this);
    gl::DeleteFramebuffers(1, &mHandle);
}

//============================================================================//

void FrameBuffer::attach(GLenum attach, const Texture2D& tex)
{
    gl::NamedFramebufferTexture(mHandle, attach, tex.get_handle(), 0);
}

void FrameBuffer::attach(GLenum attach, const TextureCube& tex, uint face)
{
    SQASSERT(face < 6u, "invalid face");
    gl::NamedFramebufferTextureLayer(mHandle, attach, tex.get_handle(), 0, int(face));
}

void FrameBuffer::attach(GLenum attach, const TextureArray2D& tex, uint layer)
{
    SQASSERT(layer < tex.get_size().z, "invalid layer");
    gl::NamedFramebufferTextureLayer(mHandle, attach, tex.get_handle(), 0, int(layer));
}

void FrameBuffer::attach(GLenum attach, const TextureArrayCube& tex, uint layer, uint face)
{
    SQASSERT(layer < tex.get_size().z && face < 6u, "invalid layer or face");
    gl::NamedFramebufferTextureLayer(mHandle, attach, tex.get_handle(), 0, int(6u * layer + face));
}

//============================================================================//

void FrameBuffer::draw_buffers(std::initializer_list<GLenum> drawBufs)
{
    gl::NamedFramebufferDrawBuffers(mHandle, drawBufs.size(), drawBufs.begin());
}

//============================================================================//

void FrameBuffer::blit(const FrameBuffer& other, Vec2U size, GLenum mask) const
{
    gl::BlitNamedFramebuffer(mHandle, other.mHandle, 0, 0, size.x, size.y, 0, 0, size.x, size.y, mask, gl::NEAREST);
}

void FrameBuffer::blit(const FrameBuffer& other, Vec2U sizeA, Vec2U sizeB, GLenum mask, GLenum filter) const
{
    gl::BlitNamedFramebuffer(mHandle, other.mHandle, 0, 0, sizeA.x, sizeA.y, 0, 0, sizeB.x, sizeB.y, mask, filter);
}
