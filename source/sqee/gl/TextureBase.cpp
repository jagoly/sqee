// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/gl/TextureBase.hpp>

#include <map>

#include <sqee/debug/Assert.hpp>
#include <sqee/gl/Context.hpp>

#include <sqee/redist/gl_loader.hpp>

using namespace sq;

//============================================================================//

Texture::Texture(GLenum target, Format format)
    : mContext(Context::get()), mTarget(target), mFormat(format) {}

//============================================================================//

Texture::Texture(Texture&& other)
    : mContext(other.mContext), mTarget(other.mTarget), mFormat(other.mFormat)
{
    mContext.impl_reset_Texture(&other, this);

    mFilter = other.mFilter;
    mMipmaps = other.mMipmaps;
    mShadow = other.mShadow;
    mWrap = other.mWrap;
    mSwizzle = other.mSwizzle;
    mSize = other.mSize;

    mHandle = other.mHandle;
    other.mHandle = 0u;
}

Texture& Texture::operator=(Texture&& other)
{ std::swap(*this, other); return *this; }

//============================================================================//

void Texture::delete_object()
{
    if (mHandle != 0u)
    {
        mContext.impl_reset_Texture(this);
        gl::DeleteTextures(1, &mHandle);
        mHandle = 0u;
    }

    mSize = { 0u, 0u, 0u };
}

//============================================================================//

void Texture::set_format(Format format)
{
    SQASSERT(mHandle == 0u, "texture already created");

    mFormat = format;
}

//============================================================================//

void Texture::generate_auto_mipmaps()
{
    gl::GenerateTextureMipmap(mHandle);
}

//============================================================================//

void Texture::set_filter_mode(bool enable)
{
    mFilter = enable;
    impl_update_paramaters();
}

void Texture::set_mipmaps_mode(bool enable)
{
    mMipmaps = enable;
    impl_update_paramaters();
}

void Texture::set_shadow_mode(bool enable)
{
    mShadow = enable;
    impl_update_paramaters();
}

//============================================================================//

void Texture::set_wrap_mode(char x, char y)
{
    mWrap = {{ x, y }};
    impl_update_paramaters();
}

void Texture::set_swizzle_mode(char r, char g, char b, char a)
{
    mSwizzle = {{ r, g, b, a }};
    impl_update_paramaters();
}

//============================================================================//

void Texture::impl_create_object()
{
    SQASSERT(mFormat != Format::Undefined, "format not set");

    this->delete_object();
    gl::CreateTextures(mTarget, 1, &mHandle);
}

//============================================================================//

void Texture::impl_update_paramaters()
{
    // do nothing if texture not created
    if (mHandle == 0u) return;

    //--------------------------------------------------------//

    if (mTarget != gl::TEXTURE_2D_MULTISAMPLE)
    {
        if (!mFilter && !mMipmaps) gl::TextureParameteri(mHandle, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
        if (!mFilter &&  mMipmaps) gl::TextureParameteri(mHandle, gl::TEXTURE_MIN_FILTER, gl::NEAREST_MIPMAP_NEAREST);
        if ( mFilter && !mMipmaps) gl::TextureParameteri(mHandle, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
        if ( mFilter &&  mMipmaps) gl::TextureParameteri(mHandle, gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR);

        gl::TextureParameteri(mHandle, gl::TEXTURE_MAG_FILTER, mFilter ? gl::LINEAR : gl::NEAREST);
    }

    //--------------------------------------------------------//

    const auto set_wrap_param = [this](GLenum pname, char param)
    {
        switch (param) {
        case 'C': gl::TextureParameteri(mHandle, pname, gl::CLAMP_TO_EDGE);   break;
        case 'R': gl::TextureParameteri(mHandle, pname, gl::REPEAT);          break;
        case 'M': gl::TextureParameteri(mHandle, pname, gl::MIRRORED_REPEAT); break;
        default: SQASSERT(false, "invalid wrap paramater"); }
    };

    if (mTarget != gl::TEXTURE_2D_MULTISAMPLE)
    {
        set_wrap_param(gl::TEXTURE_WRAP_S, mWrap[0]);
        set_wrap_param(gl::TEXTURE_WRAP_T, mWrap[1]);
    }

    //--------------------------------------------------------//

    const auto set_swizzle_param = [this](GLenum pname, char param)
    {
        switch (param) {
        case '0': gl::TextureParameteri(mHandle, pname, gl::ZERO);  break;
        case '1': gl::TextureParameteri(mHandle, pname, gl::ONE);   break;
        case 'R': gl::TextureParameteri(mHandle, pname, gl::RED);   break;
        case 'G': gl::TextureParameteri(mHandle, pname, gl::GREEN); break;
        case 'B': gl::TextureParameteri(mHandle, pname, gl::BLUE);  break;
        case 'A': gl::TextureParameteri(mHandle, pname, gl::ALPHA); break;
        default: SQASSERT(false, "invalid swizzle paramater"); }
    };

    set_swizzle_param(gl::TEXTURE_SWIZZLE_R, mSwizzle[0]);
    set_swizzle_param(gl::TEXTURE_SWIZZLE_G, mSwizzle[1]);
    set_swizzle_param(gl::TEXTURE_SWIZZLE_B, mSwizzle[2]);
    set_swizzle_param(gl::TEXTURE_SWIZZLE_A, mSwizzle[3]);

    //--------------------------------------------------------//

    if (mShadow == true)
    {
        gl::TextureParameteri(mHandle, gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    }
}

//============================================================================//

Texture::Format Texture::impl_string_to_format(const String& arg)
{
    static const std::map<String, Format> map
    {
        { "R8_UN", Format::R8_UN },
        { "RG8_UN", Format::RG8_UN },
        { "RGB8_UN", Format::RGB8_UN },
        { "RGBA8_UN", Format::RGBA8_UN },
        { "R8_SN", Format::R8_SN },
        { "RG8_SN", Format::RG8_SN },
        { "RGB8_SN", Format::RGB8_SN },
        { "RGBA8_SN", Format::RGBA8_SN },
    };

    return map.at(arg);
}
