#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq {

//============================================================================//

class Context; // Forward Declaration

//============================================================================//

/// OpenGL Texture Base Class
class Texture : public MoveOnly
{
public:

    //========================================================//

    enum class Format : uint8_t
    {
        // unsigned normalised /////
        R8_UN,  RG8_UN,  RGB8_UN,  RGBA8_UN,
        R16_UN, RG16_UN, RGB16_UN, RGBA16_UN,

        // signed normalised /////
        R8_SN,  RG8_SN,  RGB8_SN,  RGBA8_SN,
        R16_SN, RG16_SN, RGB16_SN, RGBA16_SN,

        // unsigned integral /////
        R8_UI,  RG8_UI,  RGB8_UI,  RGBA8_UI,
        R16_UI, RG16_UI, RGB16_UI, RGBA16_UI,
        R32_UI, RG32_UI, RGB32_UI, RGBA32_UI,

        // signed integral /////
        R8_SI,  RG8_SI,  RGB8_SI,  RGBA8_SI,
        R16_SI, RG16_SI, RGB16_SI, RGBA16_SI,
        R32_SI, RG32_SI, RGB32_SI, RGBA32_SI,

        // floating point /////
        R16_FP, RG16_FP, RGB16_FP, RGBA16_FP,
        R32_FP, RG32_FP, RGB32_FP, RGBA32_FP,

        // depth, stencil /////
        DEPTH16, STENCIL8, DEP24S8
    };

    //========================================================//

    /// Move Constructor
    Texture(Texture&& other);

    /// Destructor
    ~Texture() { delete_object(); }

    //========================================================//

    /// Manually delete the GL object
    void delete_object();

    /// Generate mipmaps for the texture
    void generate_auto_mipmaps();

    /// Return the size of the texture
    Vec3U get_size() const { return mSize; }

    /// Return the texture target
    GLenum get_target() const { return mTarget; }

    /// Return the texture format
    Format get_format() const { return mFormat; }

    /// Get the OpenGL handle
    GLuint get_handle() const { return mHandle; }

    //========================================================//

    void set_filter_mode(bool enable);
    void set_mipmaps_mode(bool enable);
    void set_shadow_mode(bool enable);

    //========================================================//

    void set_wrap_mode(char x, char y);
    void set_swizzle_mode(char r, char g, char b, char a);

protected:

    //========================================================//

    Texture(GLenum target, Format format);

    //========================================================//

    const GLenum mTarget;
    const Format mFormat;

    //========================================================//

    bool mFilter = false;
    bool mMipmaps = false;
    bool mShadow = false;

    //========================================================//

    array<char, 2> mWrap {{ 'C', 'C' }};
    array<char, 4> mSwizzle {{ 'R', 'G', 'B', 'A' }};

    //========================================================//

    Vec3U mSize {0u, 0u, 0u};

    //========================================================//

    GLuint mHandle = 0u;
    Context& mContext;

    //========================================================//

    void impl_create_object();
    void impl_update_paramaters();

    //========================================================//

    static Format impl_string_to_format(string str);
};

//============================================================================//

} // namespace sq
