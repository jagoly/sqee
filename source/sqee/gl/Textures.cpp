#include <sqee/gl/Textures.hpp>

#include <sqee/core/Utilities.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Functions.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/redist/stb_image.hpp>

using namespace sq;

//============================================================================//

Texture::Texture(Texture&& other) noexcept
{
    *this = std::move(other);
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    std::swap(mHandle, other.mHandle);
    return *this;
}

Texture::~Texture() noexcept
{
    if (mHandle) gl::DeleteTextures(1, &mHandle);
}

//============================================================================//

struct { int16_t group, channels; GLenum baseFormat, pixelFormat, dataType; }
static const impl_format_meta[]
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

    { 12, 0, gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 0u },
    { 12, 0, gl::STENCIL_INDEX,   gl::STENCIL_INDEX8,    0u },
    { 12, 0, gl::DEPTH_STENCIL,   gl::DEPTH24_STENCIL8,  0u },
};

//============================================================================//

inline TexFormat impl_string_to_format(const String& arg)
{
    if (arg == "R8_UN") return TexFormat::R8_UN;
    if (arg == "RG8_UN") return TexFormat::RG8_UN;
    if (arg == "RGB8_UN") return TexFormat::RGB8_UN;
    if (arg == "RGBA8_UN") return TexFormat::RGBA8_UN;

    if (arg == "R8_SN") return TexFormat::R8_SN;
    if (arg == "RG8_SN") return TexFormat::RG8_SN;
    if (arg == "RGB8_SN") return TexFormat::RGB8_SN;
    if (arg == "RGBA8_SN") return TexFormat::RGBA8_SN;

    log_warning("invalid texture format '%s'", arg);

    return TexFormat::RGBA8_UN;
}

//============================================================================//

static auto impl_load_image(TexFormat format, const String& path)
{
    struct Image
    {
        std::unique_ptr<uint8_t[], std::integral_constant<void(*)(void*), stbi_image_free>> data;
        Vec2U size;
    };

    const auto& meta = impl_format_meta[uint8_t(format)];
    SQASSERT(meta.group == 0 || meta.group == 2, "format not yet supported");

    int width = 0, height = 0, channels = 0;
    uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (data == nullptr) log_error_multiline("Error loading image '{}':\n{}", path, stbi_failure_reason());
    SQASSERT(channels == meta.channels, "image channel count does not match format");

    if (meta.group == 0) {} // 8 bit unsigned normal

    else if (meta.group == 2) // 8 bit signed normal
    {
        int8_t* cast = reinterpret_cast<int8_t*>(data);
        for (auto p = cast; p != cast + width*height*channels; ++p)
            *p += *p >= 0 ? -128 : +128;
    }

    return Image { decltype(Image::data)(data), Vec2U(Vec2I(width, height)) };
}

//============================================================================//

void Texture::generate_auto_mipmaps()
{
    gl::GenerateTextureMipmap(mHandle);
}

//============================================================================//

void Texture::set_filter_mode(bool filter, bool mipmaps)
{
    SQASSERT(mHandle != 0u, "not created");

    if (!mipmaps && !filter) gl::TextureParameteri(mHandle, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
    if (!mipmaps &&  filter) gl::TextureParameteri(mHandle, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
    if ( mipmaps && !filter) gl::TextureParameteri(mHandle, gl::TEXTURE_MIN_FILTER, gl::NEAREST_MIPMAP_NEAREST);
    if ( mipmaps &&  filter) gl::TextureParameteri(mHandle, gl::TEXTURE_MIN_FILTER, gl::LINEAR_MIPMAP_LINEAR);

    gl::TextureParameteri(mHandle, gl::TEXTURE_MAG_FILTER, filter ? gl::LINEAR : gl::NEAREST);
}

void Texture::set_shadow_mode(CompareFunc func)
{
    SQASSERT(mHandle != 0u, "not created");

    gl::TextureParameteri(mHandle, gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    gl::TextureParameteri(mHandle, gl::TEXTURE_COMPARE_FUNC, GLenum(func));
}

//============================================================================//

void Texture::set_swizzle_mode(char r, char g, char b, char a)
{
    SQASSERT(mHandle != 0u, "not created");

    const auto char_to_param = [](char ch) -> GLint
    {
        if (ch == '0') return gl::ZERO;
        if (ch == '1') return gl::ONE;
        if (ch == 'R') return gl::RED;
        if (ch == 'G') return gl::GREEN;
        if (ch == 'B') return gl::BLUE;
        if (ch == 'A') return gl::ALPHA;
        SQASSERT(false, "invalid swizzle mode");
        return -1;
    };

    gl::TextureParameteri(mHandle, gl::TEXTURE_SWIZZLE_R, char_to_param(r));
    gl::TextureParameteri(mHandle, gl::TEXTURE_SWIZZLE_G, char_to_param(g));
    gl::TextureParameteri(mHandle, gl::TEXTURE_SWIZZLE_B, char_to_param(b));
    gl::TextureParameteri(mHandle, gl::TEXTURE_SWIZZLE_A, char_to_param(a));
}

void Texture::set_wrap_mode(char x, char y)
{
    SQASSERT(mHandle != 0u, "not created");

    const auto char_to_param = [](char ch) -> GLint
    {
        if (ch == 'C') return gl::CLAMP_TO_EDGE;
        if (ch == 'R') return gl::REPEAT;
        if (ch == 'M') return gl::MIRRORED_REPEAT;
        SQASSERT(false, "invalid wrap mode");
        return -1;
    };

    gl::TextureParameteri(mHandle, gl::TEXTURE_WRAP_S, char_to_param(x));
    gl::TextureParameteri(mHandle, gl::TEXTURE_WRAP_T, char_to_param(y));
}

//============================================================================//

void Texture2D::allocate_storage(TexFormat format, Vec2U size, bool mipmaps)
{
    SQASSERT(mHandle == 0u, "already created");
    gl::CreateTextures(gl::TEXTURE_2D, 1u, &mHandle);

    const auto& meta = impl_format_meta[uint8_t(format)];
    const int levels = 1 + (mipmaps ? int(std::floor(std::log2(std::max(size.x, size.y)))) : 0);

    gl::TextureStorage2D(mHandle, levels, meta.pixelFormat, int(size.x), int(size.y));
}

void TextureArray::allocate_storage(TexFormat format, Vec3U size, bool mipmaps)
{
    SQASSERT(mHandle == 0u, "already created");
    gl::CreateTextures(gl::TEXTURE_2D_ARRAY, 1u, &mHandle);

    const auto& meta = impl_format_meta[uint8_t(format)];
    const int levels = 1 + (mipmaps ? int(std::floor(std::log2(std::max(size.x, size.y)))) : 0);

    gl::TextureStorage3D(mHandle, levels, meta.pixelFormat, int(size.x), int(size.y), int(size.z));
}

void TextureCube::allocate_storage(TexFormat format, uint size, bool mipmaps)
{
    SQASSERT(mHandle == 0u, "already created");
    gl::CreateTextures(gl::TEXTURE_CUBE_MAP, 1u, &mHandle);

    const auto& meta = impl_format_meta[uint8_t(format)];
    const int levels = 1 + (mipmaps ? int(std::floor(std::log2(size))) : 0);

    gl::TextureStorage2D(mHandle, levels, meta.pixelFormat, int(size), int(size));
}

void TextureMulti::allocate_storage(TexFormat format, Vec2U size, uint samples)
{
    SQASSERT(mHandle == 0u, "already created");
    gl::CreateTextures(gl::TEXTURE_2D_MULTISAMPLE, 1u, &mHandle);

    const auto& meta = impl_format_meta[uint8_t(format)];

    gl::TextureStorage2DMultisample(mHandle, int(samples), meta.pixelFormat, int(size.x), int(size.y), false);
}

//============================================================================//

void Texture2D::load_memory(TexFormat format, uint level, Vec2U offset, Vec2U size, const void* data)
{
    SQASSERT(mHandle != 0u, "not created");

    const auto& meta = impl_format_meta[uint8_t(format)];

    gl::TextureSubImage2D(mHandle, int(level), int(offset.x), int(offset.y),
                          int(size.x), int(size.y), meta.baseFormat, meta.dataType, data);
}

void TextureArray::load_memory(TexFormat format, uint level, Vec3U offset, Vec3U size, const void* data)
{
    SQASSERT(mHandle != 0u, "not created");

    const auto& meta = impl_format_meta[uint8_t(format)];

    gl::TextureSubImage3D(mHandle, int(level), int(offset.x), int(offset.y), int(offset.z),
                          int(size.x), int(size.y), int(size.z), meta.baseFormat, meta.dataType, data);
}

void TextureCube::load_memory(TexFormat format, uint level, Vec3U offset, Vec3U size, const void* data)
{
    SQASSERT(mHandle != 0u, "not created");

    const auto& meta = impl_format_meta[uint8_t(format)];

    gl::TextureSubImage3D(mHandle, int(level), int(offset.x), int(offset.y), int(offset.z),
                          int(size.x), int(size.y), int(size.z), meta.baseFormat, meta.dataType, data);
}

//============================================================================//

void Texture2D::load_memory(TexFormat format, Vec2U size, const void* data)
{
    load_memory(format, 0u, {0u, 0u}, size, data);
}

void TextureCube::load_memory(TexFormat format, uint face, uint size, const void* data)
{
    load_memory(format, 0u, {0u, 0u, face}, {size, size, 1u}, data);
}

void TextureArray::load_memory(TexFormat format, uint layer, Vec2U size, const void* data)
{
    load_memory(format, 0u, {0u, 0u, layer}, {size.x, size.y, 1u}, data);
}

//============================================================================//

void Texture2D::load_automatic(const String& path)
{
    const JsonValue json = parse_json_from_file(path + ".json");

    const TexFormat format = impl_string_to_format(json.at("format"));
    const bool mipmaps = json.at("mipmaps");

    const auto image = impl_load_image(format, path + ".png");

    allocate_storage(format, image.size, mipmaps);
    load_memory(format, 0u, {0u, 0u}, image.size, image.data.get());
    if (mipmaps == true) generate_auto_mipmaps();

    const bool filter = json.at("filter");
    const String& swizzle = json.at("swizzle");
    const String& wrap = json.at("wrap");

    set_filter_mode(filter, mipmaps);
    set_swizzle_mode(swizzle[0], swizzle[1], swizzle[2], swizzle[3]);
    set_wrap_mode(wrap[0], wrap[1]);
}

void TextureCube::load_automatic(const String& path)
{
    const JsonValue json = parse_json_from_file(path + "/TextureCube.json");

    const TexFormat format = impl_string_to_format(json.at("format"));
    const uint size = json.at("size");
    const bool mipmaps = json.at("mipmaps");

    allocate_storage(format, size, mipmaps);
    for (const char* fname : {"0_right", "1_left", "2_forward", "3_back", "4_up", "5_down"})
    {
        const auto image = impl_load_image(format, fmt::format("{}/{}.png", path, fname));
        SQASSERT(image.size == Vec2U(size), "image size does not match json");
        load_memory(format, uint(*fname - '0'), size, image.data.get());
    }
    if (mipmaps == true) generate_auto_mipmaps();

    const bool filter = json.at("filter");
    const String& swizzle = json.at("swizzle");
    const String& wrap = json.at("wrap");

    set_filter_mode(filter, mipmaps);
    set_swizzle_mode(swizzle[0], swizzle[1], swizzle[2], swizzle[3]);
    set_wrap_mode(wrap[0], wrap[1]);
}

void TextureArray::load_automatic(const String& path)
{
    const JsonValue json = parse_json_from_file(path + "/TextureArray.json");

    const TexFormat format = impl_string_to_format(json.at("format"));
    const uint width = json.at("width");
    const uint height = json.at("height");
    const uint layers = json.at("layers");
    const bool mipmaps = json.at("mipmaps");

    allocate_storage(format, {width, height, layers}, mipmaps);
    for (uint layer = 0u; layer < layers; ++layer)
    {
        const uint digits = layers > 10u ? layers > 100u ? 3u : 2u : 1u;
        const auto image = impl_load_image(format, fmt::format("{}/{:0>{}}.png", path, layer, digits));
        SQASSERT(image.size == Vec2U(width, height), "image size does not match json");
        load_memory(format, layer, {width, height}, image.data.get());
    }
    if (mipmaps == true) generate_auto_mipmaps();

    const bool filter = json.at("filter");
    const String& swizzle = json.at("swizzle");
    const String& wrap = json.at("wrap");

    set_filter_mode(filter, mipmaps);
    set_swizzle_mode(swizzle[0], swizzle[1], swizzle[2], swizzle[3]);
    set_wrap_mode(wrap[0], wrap[1]);
}

//============================================================================//

// todo: get rid of this garbage

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
    const auto image = impl_load_image(format, path);

    //========================================================//

    auto texture = Texture2D();

    texture.allocate_storage(format, image.size, true);
    texture.load_memory(format, image.size, image.data.get());
    texture.generate_auto_mipmaps();

    texture.set_filter_mode(true, true);
    texture.set_swizzle_mode(swizzle[0], swizzle[1], swizzle[2], swizzle[3]);
    texture.set_wrap_mode(wrap[0], wrap[1]);

    return texture;
}
