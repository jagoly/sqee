#include <fstream>

#include <sqee/assert.hpp>

#include <sqee/redist/gl_loader.hpp>
#include <sqee/redist/stb_image.hpp>
#include <sqee/redist/nl_json.hpp>

#include <sqee/debug/Logging.hpp>

#include <sqee/misc/Files.hpp>

#include <sqee/gl/Textures.hpp>

using namespace sq;
using JsonValue = nlohmann::json;

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

inline void impl_to_signed_norm_16(uint16_t* data, int count)
{
    for (int i = 0; i < count; ++i)
    {
        int16_t& cast = reinterpret_cast<int16_t&>(data[i]);
        if (cast >= 0) cast -= 32768; else cast += 32768;
    }
}

//============================================================================//

struct Impl_Load_Image : NonCopyable
{
    void* data = nullptr;
    Vec2U size = { 0u, 0u };

    //--------------------------------------------------------//

    Impl_Load_Image(Texture::Format format, const string& path)
    {
//        // prepend assets directory to the path
//        if (path.is_relative()) path = "assets" / path;

//        // automatically add png or jpg file extension
//        if (path.has_extension() == false)
//        {
//            const fs::path pathPNG = path.string() + ".png";
//            const fs::path pathJPG = path.string() + ".jpg";

//            const bool existsPNG = fs::exists(pathPNG);
//            const bool existsJPG = fs::exists(pathJPG);

//            if (existsPNG && existsJPG) log_warning("ambiguous image path '%s'", path);
//            else if (existsPNG) path = pathPNG; else if (existsJPG) path = pathJPG;
//        }

//        log_assert(fs::exists(path), "file does not exist: %s", fs::absolute(path));

        string fullPath = "assets/" + path;
        if (check_file_exists(fullPath+".png")) fullPath += ".png";
        else if (check_file_exists(fullPath+".jpg")) fullPath += ".jpg";
        else log_error("Failed to find image %s", fullPath);

        //--------------------------------------------------------//

        const auto& meta = impl_Format_Meta[uint8_t(format)];

        SQASSERT(meta.group == 0 || meta.group == 2, "fixme");

        //--------------------------------------------------------//

        Vec2I size = { 0, 0 };
        int channels = 0;

        data = stbi_load(fullPath.c_str(), &size.x, &size.y, &channels, 0);
        log_assert(data != NULL, "Failed to load image from %s", fullPath);

        SQASSERT(channels == meta.channels, "");

        if (meta.group == 2)
        {
            uint8_t* const unormPtr = static_cast<uint8_t*>(data);
            impl_to_signed_norm_8(unormPtr, size.x * size.y * channels);
        }

        //--------------------------------------------------------//

        this->size = Vec2U(size);
    }

    //--------------------------------------------------------//

    ~Impl_Load_Image()
    {
        if (data != nullptr) free(data);
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

void Texture2D::load_file(const string& path)
{
    Impl_Load_Image image (mFormat, path);
    SQASSERT(image.size == Vec2U(mSize), "");

    load_memory(image.data);
}

void TextureCube::load_file(const string& path, uint face)
{
    Impl_Load_Image image (mFormat, path);
    SQASSERT(image.size == Vec2U(mSize), "");

    load_memory(image.data, face);
}

void TextureCube::load_directory(const string& path)
{
    load_file(path + "/0_right",   0u);
    load_file(path + "/1_left",    1u);
    load_file(path + "/2_forward", 2u);
    load_file(path + "/3_back",    3u);
    load_file(path + "/4_up",      4u);
    load_file(path + "/5_down",    5u);
}

void TextureArray2D::load_file(const string& path, uint layer)
{
    Impl_Load_Image image (mFormat, path);
    SQASSERT(image.size == Vec2U(mSize), "");

    load_memory(image.data, layer);
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

Texture2D Texture2D::make_from_package(const string& path)
{
    const string::size_type splitPos = path.find(':');
    log_assert(splitPos != string::npos, "bad path '%s'", path);

    const string package = path.substr(0u, splitPos);
    const string name = path.substr(splitPos + 1u);

    std::ifstream srcFile("assets/packages/" + package + "/textures.json");
    const auto json = JsonValue::parse(srcFile).at(name);

    //========================================================//

    const string wrap = json.at("wrap");
    const string swizzle = json.at("swizzle");

    SQASSERT(wrap.size() == 2u, "");
    SQASSERT(swizzle.size() == 4u, "");

    //========================================================//

    const auto format = impl_string_to_format(json.at("format"));

    Impl_Load_Image image (format, "packages/" + package + "/textures/" + name);

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
