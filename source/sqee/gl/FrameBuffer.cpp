#include <sqee/gl/FrameBuffer.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/gl/Functions.hpp>
#include <sqee/gl/Textures.hpp>

using namespace sq;

//============================================================================//

FrameBuffer::FrameBuffer()
{
    gl::CreateFramebuffers(1, &mHandle);
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
{
    *this = std::move(other);
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
{
    std::swap(mHandle, other.mHandle);
    return *this;
}

FrameBuffer::~FrameBuffer() noexcept
{
    if (mHandle) gl::DeleteFramebuffers(1, &mHandle);
}

//============================================================================//

void FrameBuffer::attach(FboAttach attach, const Texture2D& tex)
{
    SQASSERT(tex.get_handle(), "");
    gl::NamedFramebufferTexture(mHandle, GLenum(attach), tex.get_handle(), 0);
}

void FrameBuffer::attach(FboAttach attach, const TextureCube& tex, uint face)
{
    SQASSERT(tex.get_handle() && face < 6u, "");
    gl::NamedFramebufferTextureLayer(mHandle, GLenum(attach), tex.get_handle(), 0, int(face));
}

void FrameBuffer::attach(FboAttach attach, const TextureArray& tex, uint layer)
{
    SQASSERT(tex.get_handle(), "");
    gl::NamedFramebufferTextureLayer(mHandle, GLenum(attach), tex.get_handle(), 0, int(layer));
}

void FrameBuffer::attach(FboAttach attach, const TextureMulti& tex)
{
    SQASSERT(tex.get_handle(), "");
    gl::NamedFramebufferTexture(mHandle, GLenum(attach), tex.get_handle(), 0);
}

//============================================================================//

void FrameBuffer::draw_buffers(std::initializer_list<FboAttach> drawBufs)
{
    gl::NamedFramebufferDrawBuffers(mHandle, int(drawBufs.size()),
                                    reinterpret_cast<const GLenum*>(drawBufs.begin()));
}

//============================================================================//

void FrameBuffer::blit(const FrameBuffer& other, Vec2U size, BlitMask mask) const
{
    gl::BlitNamedFramebuffer(mHandle, other.mHandle, 0, 0, int(size.x), int(size.y),
                             0, 0, int(size.x), int(size.y), GLenum(mask), gl::NEAREST);
}

void FrameBuffer::blit(const FrameBuffer& other, Vec2U sizeA, Vec2U sizeB, BlitMask mask, bool filter) const
{
    gl::BlitNamedFramebuffer(mHandle, other.mHandle, 0, 0, int(sizeA.x), int(sizeA.y),
                             0, 0, int(sizeB.x), int(sizeB.y), GLenum(mask), filter ? gl::LINEAR : gl::NEAREST);
}
