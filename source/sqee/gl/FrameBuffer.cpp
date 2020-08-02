#include <sqee/gl/FrameBuffer.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/redist/gl_loader.hpp>

using namespace sq;

//============================================================================//

FrameBuffer::FrameBuffer() : mContext(Context::get())
{
    gl::CreateFramebuffers(1, &mHandle);
}

//============================================================================//

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : mContext(other.mContext)
{
    mContext.impl_reset_FrameBuffer(&other, this);
    mHandle = other.mHandle; other.mHandle = 0u;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
{ std::swap(*this, other); return *this; }

//============================================================================//

FrameBuffer::~FrameBuffer() noexcept
{
    mContext.impl_reset_FrameBuffer(this);
    gl::DeleteFramebuffers(1, &mHandle);
}

//============================================================================//

void FrameBuffer::attach(GLenum attach, const Texture2D& tex)
{
    SQASSERT(tex.get_handle(), "");
    gl::NamedFramebufferTexture(mHandle, attach, tex.get_handle(), 0);
}

void FrameBuffer::attach(GLenum attach, const TextureCube& tex, uint face)
{
    SQASSERT(tex.get_handle() && face < 6u, "");
    gl::NamedFramebufferTextureLayer(mHandle, attach, tex.get_handle(), 0, int(face));
}

void FrameBuffer::attach(GLenum attach, const TextureArray2D& tex, uint layer)
{
    SQASSERT(tex.get_handle() && layer < tex.get_size().z, "");
    gl::NamedFramebufferTextureLayer(mHandle, attach, tex.get_handle(), 0, int(layer));
}

//void FrameBuffer::attach(GLenum attach, const TextureArrayCube& tex, uint layer, uint face)
//{
//    SQASSERT(tex.get_handle() && layer < tex.get_size().z && face < 6u, "");
//    gl::NamedFramebufferTextureLayer(mHandle, attach, tex.get_handle(), 0, int(6u * layer + face));
//}

void FrameBuffer::attach(GLenum attach, const TextureMulti& tex)
{
    SQASSERT(tex.get_handle(), "");
    gl::NamedFramebufferTexture(mHandle, attach, tex.get_handle(), 0);
}

//============================================================================//

void FrameBuffer::draw_buffers(std::initializer_list<GLenum> drawBufs)
{
    gl::NamedFramebufferDrawBuffers(mHandle, int(drawBufs.size()), drawBufs.begin());
}

//============================================================================//

void FrameBuffer::blit(const FrameBuffer& other, Vec2U size, GLenum mask) const
{
    gl::BlitNamedFramebuffer ( mHandle, other.mHandle, 0, 0, int(size.x), int(size.y),
                               0, 0, int(size.x), int(size.y), mask, gl::NEAREST );
}

void FrameBuffer::blit(const FrameBuffer& other, Vec2U sizeA, Vec2U sizeB, GLenum mask, GLenum filter) const
{
    gl::BlitNamedFramebuffer ( mHandle, other.mHandle, 0, 0, int(sizeA.x), int(sizeA.y),
                               0, 0, int(sizeB.x), int(sizeB.y), mask, filter );
}
