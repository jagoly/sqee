#include <sqee/gl/Textures.hpp>

#include <sqee/core/Utilities.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/redist/gl_loader.hpp>
#include <sqee/redist/stb_image.hpp>

using namespace sq;

//============================================================================//

Texture::Texture(GLenum target, Format format)
    : mContext(Context::get()), mTarget(target), mFormat(format) {}

Texture::Texture(Texture&& other) noexcept
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

Texture& Texture::operator=(Texture&& other) noexcept
{
    std::swap(*this, other);
    return *this;
}

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

void Texture::set_format(Format format)
{
    SQASSERT(mHandle == 0u, "texture already created");

    mFormat = format;
}

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

    delete_object();
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

namespace { // anonymous

//============================================================================//

static constexpr struct { const int16_t group, channels;
                          const GLenum baseFormat, pixelFormat;
                          const GLenum dataType; }
impl_Format_Meta[]
{
    { 0, 1, gl::RED,  gl::R8,    gl::UNSIGNED_BYTE },
    { 0, 2, gl::RG,   gl::RG8,   gl::UNSIGNED_BYTE },
    { 0, 3, gl::RGB,  gl::RGB8,  gl::UNSIGNED_BYTE },
    { 0, 4, gl::RGBA, gl::RGBA8, gl::UNSIGNED_BYTE },

    { 1, 1, gl::RED,  gl::R16,    gl::UNSIGNED_SHORT },
    { 1, 2, gl::RG,   gl::RG16,   gl::UNSIGNED_SHORT },
    { 1, 3, gl::RGB,  gl::RGB16,  gl::UNSIGNED_SHORT },
    { 1, 4, gl::RGBA, gl::RGBA16, gl::UNSIGNED_SHORT },

    { 2, 1, gl::RED,  gl::R8_SNORM,    gl::BYTE },
    { 2, 2, gl::RG,   gl::RG8_SNORM,   gl::BYTE },
    { 2, 3, gl::RGB,  gl::RGB8_SNORM,  gl::BYTE },
    { 2, 4, gl::RGBA, gl::RGBA8_SNORM, gl::BYTE },

    { 3, 1, gl::RED,  gl::R16_SNORM,    gl::SHORT },
    { 3, 2, gl::RG,   gl::RG16_SNORM,   gl::SHORT },
    { 3, 3, gl::RGB,  gl::RGB16_SNORM,  gl::SHORT },
    { 3, 4, gl::RGBA, gl::RGBA16_SNORM, gl::SHORT },

    { 4, 1, gl::RED,  gl::R8UI,    gl::UNSIGNED_BYTE },
    { 4, 2, gl::RG,   gl::RG8UI,   gl::UNSIGNED_BYTE },
    { 4, 3, gl::RGB,  gl::RGB8UI,  gl::UNSIGNED_BYTE },
    { 4, 4, gl::RGBA, gl::RGBA8UI, gl::UNSIGNED_BYTE },

    { 5, 1, gl::RED,  gl::R16UI,    gl::UNSIGNED_SHORT },
    { 5, 2, gl::RG,   gl::RG16UI,   gl::UNSIGNED_SHORT },
    { 5, 3, gl::RGB,  gl::RGB16UI,  gl::UNSIGNED_SHORT },
    { 5, 4, gl::RGBA, gl::RGBA16UI, gl::UNSIGNED_SHORT },

    { 6, 1, gl::RED,  gl::R32UI,    gl::UNSIGNED_INT },
    { 6, 2, gl::RG,   gl::RG32UI,   gl::UNSIGNED_INT },
    { 6, 3, gl::RGB,  gl::RGB32UI,  gl::UNSIGNED_INT },
    { 6, 4, gl::RGBA, gl::RGBA32UI, gl::UNSIGNED_INT },

    { 7, 1, gl::RED,  gl::R8I,    gl::BYTE },
    { 7, 2, gl::RG,   gl::RG8I,   gl::BYTE },
    { 7, 3, gl::RGB,  gl::RGB8I,  gl::BYTE },
    { 7, 4, gl::RGBA, gl::RGBA8I, gl::BYTE },

    { 8, 1, gl::RED,  gl::R16I,    gl::SHORT },
    { 8, 2, gl::RG,   gl::RG16I,   gl::SHORT },
    { 8, 3, gl::RGB,  gl::RGB16I,  gl::SHORT },
    { 8, 4, gl::RGBA, gl::RGBA16I, gl::SHORT },

    { 9, 1, gl::RED,  gl::R32I,    gl::INT },
    { 9, 2, gl::RG,   gl::RG32I,   gl::INT },
    { 9, 3, gl::RGB,  gl::RGB32I,  gl::INT },
    { 9, 4, gl::RGBA, gl::RGBA32I, gl::INT },

    { 10, 1, gl::RED,  gl::R16F,    gl::HALF_FLOAT },
    { 10, 2, gl::RG,   gl::RG16F,   gl::HALF_FLOAT },
    { 10, 3, gl::RGB,  gl::RGB16F,  gl::HALF_FLOAT },
    { 10, 4, gl::RGBA, gl::RGBA16F, gl::HALF_FLOAT },

    { 11, 1, gl::RED,  gl::R32F,    gl::FLOAT },
    { 11, 2, gl::RG,   gl::RG32F,   gl::FLOAT },
    { 11, 3, gl::RGB,  gl::RGB32F,  gl::FLOAT },
    { 11, 4, gl::RGBA, gl::RGBA32F, gl::FLOAT },

    { 12, 0, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 0 },
    { 12, 0, gl::STENCIL_INDEX,   gl::STENCIL_INDEX8,    0 },
    { 12, 0, gl::DEPTH_STENCIL,   gl::DEPTH24_STENCIL8,  0 },
};

//============================================================================//

inline int impl_calculate_mip_levels(bool enable, uint size)
{
    if (enable == false) return 1;
    return 1 + int(std::floor(std::log2(size)));
}

inline int impl_calculate_mip_levels(bool enable, Vec2U size)
{
    if (enable == false) return 1;
    const uint max = maths::max(size.x, size.y);
    return 1 + int(std::floor(std::log2(max)));
}

inline void impl_to_signed_norm_8(uint8_t* data, int count)
{
    for (int i = 0; i < count; ++i)
    {
        int8_t& cast = reinterpret_cast<int8_t&>(data[i]);
        if (cast >= 0) cast -= 128; else cast += 128;
    }
}

//inline void impl_to_signed_norm_16(uint16_t* data, int count)
//{
//    for (int i = 0; i < count; ++i)
//    {
//        int16_t& cast = reinterpret_cast<int16_t&>(data[i]);
//        if (cast >= 0) cast -= 32768; else cast += 32768;
//    }
//}

//============================================================================//

inline Texture::Format impl_string_to_format(const String& arg)
{
    if (arg == "R8_UN") return Texture::Format::R8_UN;
    if (arg == "RG8_UN") return Texture::Format::RG8_UN;
    if (arg == "RGB8_UN") return Texture::Format::RGB8_UN;
    if (arg == "RGBA8_UN") return Texture::Format::RGBA8_UN;
    if (arg == "R8_SN") return Texture::Format::R8_SN;
    if (arg == "RG8_SN") return Texture::Format::RG8_SN;
    if (arg == "RGB8_SN") return Texture::Format::RGB8_SN;
    if (arg == "RGBA8_SN") return Texture::Format::RGBA8_SN;

    log_warning("invalid texture format '%s'", arg);

    return Texture::Format::RGBA8_UN;
}

//============================================================================//

struct Impl_Load_Image : NonCopyable
{
    void* data = nullptr;
    Vec2U size = { 0u, 0u };

    Impl_Load_Image(Texture::Format format, String path)
    {
        if (check_file_exists(path) == true) {}
        else if (check_file_exists(path + ".png")) path += ".png";
        else if (check_file_exists(path + ".jpg")) path += ".jpg";
        else log_error("Failed to find image '{}'", path);

        const auto& meta = impl_Format_Meta[uint8_t(format)];

        SQASSERT(meta.group == 0 || meta.group == 2, "fixme");

        Vec2I sizeS = { 0, 0 };
        int channels = 0;

        data = stbi_load(path.c_str(), &sizeS.x, &sizeS.y, &channels, 0);
        if (data == NULL) log_error("Error loading image from '{}'", path);

        SQASSERT(channels == meta.channels, "");

        if (meta.group == 2)
        {
            uint8_t* const unormPtr = static_cast<uint8_t*>(data);
            impl_to_signed_norm_8(unormPtr, sizeS.x * sizeS.y * channels);
        }

        size = Vec2U(sizeS);
    }

    ~Impl_Load_Image()
    {
        if (data != nullptr)
            free(data);
    }
};

//============================================================================//

} // anonymous namespace

//============================================================================//

// Texture Constructors /////

Texture2D::Texture2D(Format format) : Texture(gl::TEXTURE_2D, format) {}

TextureCube::TextureCube(Format format) : Texture(gl::TEXTURE_CUBE_MAP, format) {}

TextureArray2D::TextureArray2D(Format format) : Texture(gl::TEXTURE_2D_ARRAY, format) {}

//TextureArrayCube::TextureArrayCube(Format format) : Texture(gl::TEXTURE_CUBE_MAP_ARRAY, format) {}

TextureVolume::TextureVolume(Format format) : Texture(gl::TEXTURE_3D, format) {}

TextureMulti::TextureMulti(Format format) : Texture(gl::TEXTURE_2D_MULTISAMPLE, format) {}

//============================================================================//

// Allocate Storage Methods /////

void Texture2D::allocate_storage(Vec2U size)
{
    Texture::impl_create_object();
    Texture::impl_update_paramaters();

    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];
    const int levels = impl_calculate_mip_levels(mMipmaps, size);

    gl::TextureStorage2D(mHandle, levels, meta.pixelFormat, int(size.x), int(size.y));

    mSize = { size.x, size.y, 1u };
}

void TextureCube::allocate_storage(uint size)
{
    Texture::impl_create_object();
    Texture::impl_update_paramaters();

    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];
    const int levels = impl_calculate_mip_levels(mMipmaps, size);

    gl::TextureStorage2D(mHandle, levels, meta.pixelFormat, int(size), int(size));

    mSize = { size, size, 6u };
}

void TextureArray2D::allocate_storage(Vec3U size)
{
    Texture::impl_create_object();
    Texture::impl_update_paramaters();

    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];
    const int levels = impl_calculate_mip_levels(mMipmaps, Vec2U(size));

    gl::TextureStorage3D(mHandle, levels, meta.pixelFormat, int(size.x), int(size.y), int(size.z));

    mSize = { size.x, size.y, size.z };
}

void TextureVolume::allocate_storage(Vec3U size)
{
    Texture::impl_create_object();
    Texture::impl_update_paramaters();

    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];

    gl::TextureStorage3D(mHandle, 1, meta.pixelFormat, int(size.x), int(size.y), int(size.z));

    mSize = { size.x, size.y, size.z };
}

void TextureMulti::allocate_storage(Vec3U size)
{
    Texture::impl_create_object();
    Texture::impl_update_paramaters();

    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];

    gl::TextureStorage2DMultisample(mHandle, int(size.z), meta.pixelFormat, int(size.x), int(size.y), false);

    mSize = { size.x, size.y, size.z };
}

//============================================================================//

// Memory Loading Methods /////

void Texture2D::load_memory(const void* data)
{
    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];

    gl::TextureSubImage2D(mHandle, 0, 0, 0, int(mSize.x), int(mSize.y),
                          meta.baseFormat, meta.dataType, data);
}

void TextureCube::load_memory(const void* data)
{
    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];

    gl::TextureSubImage3D(mHandle, 0, 0, 0, 0, int(mSize.x), int(mSize.y), 6,
                          meta.baseFormat, meta.dataType, data);
}

void TextureCube::load_memory(const void* data, uint face)
{
    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];

    gl::TextureSubImage3D(mHandle, 0, 0, 0, int(face), int(mSize.x), int(mSize.y), 1,
                          meta.baseFormat, meta.dataType, data);
}

void TextureArray2D::load_memory(const void* data)
{
    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];

    gl::TextureSubImage3D(mHandle, 0, 0, 0, 0, int(mSize.x), int(mSize.y), int(mSize.z),
                          meta.baseFormat, meta.dataType, data);
}

void TextureArray2D::load_memory(const void* data, uint layer)
{
    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];

    gl::TextureSubImage3D(mHandle, 0, 0, 0, int(layer), int(mSize.x), int(mSize.y), 1,
                          meta.baseFormat, meta.dataType, data);
}

void TextureVolume::load_memory(const void* data, Vec3U offset, Vec3U size)
{
    const auto& meta = impl_Format_Meta[uint8_t(mFormat)];

    const int x = int(offset.x), y = int(offset.y), z = int(offset.z);
    const int w = int(size.x), h = int(size.y), d = int(size.z);

    gl::TextureSubImage3D(mHandle, 0, x, y, z, w, h, d, meta.baseFormat, meta.dataType, data);
}

//============================================================================//

// File Loading Methods /////

void Texture2D::load_file(StringView path)
{
    Impl_Load_Image image ( mFormat, String(path) );
    SQASSERT(image.size == Vec2U(mSize), "");
    load_memory(image.data);
}

void Texture2D::load_automatic(StringView path)
{
    const auto jsonPath = build_string(directory_from_path(path), "/meta.json");
    const auto fileName = String(file_from_path(path));

    const auto json = parse_json_from_file(jsonPath).at(fileName);

    const String& wrap = json.at("wrap");
    const String& swizzle = json.at("swizzle");

    set_format(impl_string_to_format(json.at("format")));
    set_wrap_mode(wrap[0], wrap[1]);
    set_swizzle_mode(swizzle[0], swizzle[1], swizzle[2], swizzle[3]);
    set_filter_mode(json.at("filter"));
    set_mipmaps_mode(json.at("mipmaps"));

    Impl_Load_Image image ( mFormat, String(path) );
    allocate_storage(image.size);
    load_memory(image.data);
    if (mMipmaps == true) generate_auto_mipmaps();
}

void TextureCube::load_file(StringView path, uint face)
{
    Impl_Load_Image image ( mFormat, String(path) );
    SQASSERT(image.size == Vec2U(mSize), "");
    load_memory(image.data, face);
}

void TextureCube::load_automatic(StringView path)
{
    const auto json = parse_json_from_file(build_string(path, "/meta.json"));

    const String& wrap = json.at("wrap");
    const String& swizzle = json.at("swizzle");

    set_format(impl_string_to_format(json.at("format")));
    set_wrap_mode(wrap[0], wrap[1]);
    set_swizzle_mode(swizzle[0], swizzle[1], swizzle[2], swizzle[3]);
    set_filter_mode(json.at("filter"));
    set_mipmaps_mode(json.at("mipmaps"));
    allocate_storage(json.at("size"));

    load_file(build_string(path, "/0_right"),   0u);
    load_file(build_string(path, "/1_left"),    1u);
    load_file(build_string(path, "/2_forward"), 2u);
    load_file(build_string(path, "/3_back"),    3u);
    load_file(build_string(path, "/4_up"),      4u);
    load_file(build_string(path, "/5_down"),    5u);

    if (mMipmaps == true) generate_auto_mipmaps();
}

void TextureArray2D::load_file(StringView path, uint layer)
{
    Impl_Load_Image image ( mFormat, String(path) );
    SQASSERT(image.size == Vec2U(mSize), "");
    load_memory(image.data, layer);
}

void TextureArray2D::load_automatic(StringView path)
{
    const auto json = parse_json_from_file(build_string(path, "/meta.json"));

    const String& wrap = json.at("wrap");
    const String& swizzle = json.at("swizzle");

    set_format(impl_string_to_format(json.at("format")));
    set_wrap_mode(wrap[0], wrap[1]);
    set_swizzle_mode(swizzle[0], swizzle[1], swizzle[2], swizzle[3]);
    set_filter_mode(json.at("filter"));
    set_mipmaps_mode(json.at("mipmaps"));
    allocate_storage({json.at("width"), json.at("height"), json.at("layers")});

    const uint digits = mSize.z > 10u ? mSize.z > 100u ? 3u : 2u : 1u;

    for (uint layer = 0u; layer < mSize.z; ++layer)
    {
        const auto filePath = fmt::format("{}/{:0>{}}", path, layer, digits);

        Impl_Load_Image image ( mFormat, filePath );
        SQASSERT(image.size == Vec2U(mSize), "");

        load_memory(image.data, layer);
    }

    if (mMipmaps == true) generate_auto_mipmaps();
}

//============================================================================//

// TextureArrayCube Methods /////

/*
void TextureCubeArray::buffer_memory(const void* _data, uint _face, uint _index, GLenum _type) {
    gl::TextureSubImage3D(tex, 0, 0, 0, _index*6+_face, size.x, size.y, 1, format, _type, _data);
}

void TextureCubeArray::buffer_file(const string& _path, uint _face, uint _index) {
    string path = static_path() + "textures/" + _path;
    if (check_file_exists(path+".png")) path += ".png";
    else if (check_file_exists(path+".jpg")) path += ".jpg";

    LoadedImage img = load_image(path, format);
    if (!img.data) log_error("Failed to load texture from %s", path);
    else buffer_memory(img.data, _face, _index, gl::UNSIGNED_BYTE);
}

void TextureCubeArray::buffer_full(const string& _path, uint _index) {
    buffer_file(_path+"/x+", 0, _index);
    buffer_file(_path+"/x-", 1, _index);
    buffer_file(_path+"/y+", 2, _index);
    buffer_file(_path+"/y-", 3, _index);
    buffer_file(_path+"/z+", 4, _index);
    buffer_file(_path+"/z-", 5, _index);
}

void TextureCubeArray::allocate_storage(Vec2U _size, bool _mipmaps) {
    create_and_setup_texture(); size = Vec3U(_size.x, _size.x, _size.y);
    int maxLevel = _mipmaps ? std::floor(std::log2(size.x)) + 1 : 1;
    gl::TextureStorage3D(tex, maxLevel, iFormat, size.x, size.y, size.z*6);
}
*/


//============================================================================//

Texture2D Texture2D::make_from_package(const String& path)
{
    const size_t splitPos = path.find(':');
    if (splitPos == String::npos) log_error("bad path '{}'", path);

    const StringView package ( path.c_str(), splitPos );
    const char* const name = path.c_str() + splitPos + 1u;

    const auto jsonPath = build_string("assets/packages/", package, "/textures.json");
    const auto json = parse_json_from_file(jsonPath).at(name);

    //========================================================//

    const String& wrap = json.at("wrap");
    const String& swizzle = json.at("swizzle");

    SQASSERT(wrap.size() == 2u, "");
    SQASSERT(swizzle.size() == 4u, "");

    //========================================================//

    const auto format = impl_string_to_format(json.at("format"));
    const auto imagePath = build_string("assets/packages/", package, "/textures/", name);

    Impl_Load_Image image ( format, imagePath );

    //========================================================//

    auto texture = Texture2D(format);

    texture.set_filter_mode(true);
    texture.set_mipmaps_mode(true);

    texture.set_wrap_mode(wrap[0], wrap[1]);
    texture.set_swizzle_mode(swizzle[0], swizzle[1], swizzle[2], swizzle[3]);

    texture.allocate_storage(image.size);

    texture.load_memory(image.data);

    texture.generate_auto_mipmaps();

    //========================================================//

    return texture;
}
