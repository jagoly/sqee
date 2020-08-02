#include <sqee/debug/Text.hpp>

#include <sqee/core/Algorithms.hpp>
#include <sqee/data/BasicFont.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/Program.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/redist/gl_loader.hpp>

using namespace sq;

//============================================================================//

static constexpr const StringView VERTEX_SHADER_SOURCE = R"glsl(

#version 330 core
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_gpu_shader5 : enable

const vec2 texcrds[6] =
{
    vec2(0, 1), vec2(0, 0), vec2(1, 1),
    vec2(1, 0), vec2(1, 1), vec2(0, 0)
};

layout(location=0) in vec3 glyph;

out vec3 texcrd;

void main()
{
    gl_Position = vec4(glyph.xy, 0.f, 1.f);
    texcrd.xy = texcrds[gl_VertexID % 6];
    texcrd.z = glyph.z;
}

)glsl";

//============================================================================//

static constexpr const StringView FRAGMENT_SHADER_SOURCE = R"glsl(

#version 330 core
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_gpu_shader5 : enable

in vec3 texcrd;

layout(location=0) uniform vec4 colour;

layout(binding=0) uniform sampler2DArray tex;

out vec4 fragment;

void main()
{
    float texel = texture(tex, texcrd).r;
    fragment = vec4(colour.rgb, texel * colour.a);
}

)glsl";

//============================================================================//

void sq::render_text_basic(StringView text, Vec2I flow, Vec2I align, Vec2F scale, Vec4F colour, bool shadow)
{
    struct StaticStuff
    {
        FixedBuffer vbo; VertexArray vao;
        TextureArray2D texFont { Texture::Format::R8_UN };
        TextureArray2D texGlow { Texture::Format::R8_UN };
        Program program;

        StaticStuff()
        {
            texFont.set_filter_mode(true);
            texGlow.set_filter_mode(true);

            vbo.allocate_dynamic(2048u * 12u, nullptr);
            vao.set_vertex_buffer(vbo, 12u);

            vao.add_float_attribute(0u, 3u, gl::FLOAT, false, 0u);

            texFont.allocate_storage({16u, 16u, 256u});
            texFont.load_memory(sqee_BasicFontMain);

            texGlow.allocate_storage({16u, 16u, 256u});
            texGlow.load_memory(sqee_BasicFontGlow);

            program.create(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);
        }
    };

    //--------------------------------------------------------//

    SQASSERT(flow.x == -1 || flow.x == +1, "invalid horizontal flow");
    SQASSERT(flow.y == -1 || flow.y == +1, "invalid vertical flow");
    SQASSERT(align.x == -1 || align.x == 0 || align.x == +1, "invalid horizontal align");
    SQASSERT(align.y == -1 || align.y == 0 || align.y == +1, "invalid vertical align");

    //--------------------------------------------------------//

    static StaticStuff stuff;

    Context& context = Context::get();

    const Vec2F viewPort = Vec2F(context.get_ViewPort());

    const Vec2F size = Vec2F(scale) / viewPort;
    const Vec2F margin = Vec2F(8.f) / viewPort;

    const float maxWidth = 1.f - margin.x * 2.f;

    //--------------------------------------------------------//

    std::vector<String> lines = { "" };

    for (uint i = 0u; i < text.size(); i++)
    {
        if (text.at(i) == '\n') { lines.emplace_back(); continue; }
        if ((lines.back().size() + 1u) * size.x * 0.75f > maxWidth) lines.emplace_back();
        lines.back().push_back(text.at(i));
    }

    //--------------------------------------------------------//

    // if flow.x == align.x, then reverse rows
    if ((flow.x == -1 && align.x == -1) || (flow.x == +1 && align.x == +1))
        for (auto& line : lines) std::reverse(line.begin(), line.end());

    // if flow.y == align.y, then reverse columns
    if ((flow.y == -1 && align.y == -1) || (flow.y == +1 && align.y == +1))
        std::reverse(lines.begin(), lines.end());

    //--------------------------------------------------------//

    // text origin and per-character offset

    Vec2F origin = Vec2F { 0.5f, 0.5f };
    Vec2F offset = Vec2F { 0.75f, 1.f } * size;

    if (align.x == +1) origin.x = 1.f - margin.x - size.x;
    else if (align.x == -1) origin.x = margin.x;
    if (align.y == +1) origin.y = 1.f - margin.y - size.y;
    else if (align.y == -1) origin.y = margin.y;

    if (align.x == +1) offset.x = -offset.x;
    if (align.y == +1) offset.y = -offset.y;

    //--------------------------------------------------------//

    std::vector<std::array<Vec3F, 6>> glyphs;

    float currentY = origin.y;

    for (const String& line : lines)
    {
        float currentX = origin.x;

        for (const char& character : line)
        {
            const Vec2F current = { currentX, currentY };
            const Vec2F min = current * 2.f - 1.f;
            const Vec2F max = (current + size) * 2.f - 1.f;
            const float index = float(character);

            const Vec3F LB = { min.x, min.y, index };
            const Vec3F LT = { min.x, max.y, index };
            const Vec3F RB = { max.x, min.y, index };
            const Vec3F RT = { max.x, max.y, index };

            glyphs.push_back( {{ LB, LT, RB, RT, RB, LT }} );

            currentX += offset.x;
        }

        currentY += offset.y;
    }

    //--------------------------------------------------------//

    stuff.vbo.update(0u, uint(glyphs.size()) * 72u, glyphs.data());

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Alpha);

    context.bind_Program(stuff.program);
    context.bind_VertexArray(stuff.vao);

    if (shadow == true)
    {
        context.bind_Texture(stuff.texGlow, 0u);

        if (maths::dot(Vec3F(colour), 0.3333f) < 0.5f)
            stuff.program.update(0, Vec4F(1.f, 1.f, 1.f, colour.a));
        else stuff.program.update(0, Vec4F(0.f, 0.f, 0.f, colour.a));

        gl::DrawArrays(gl::TRIANGLES, 0, int(glyphs.size()) * 6);
    }

    context.bind_Texture(stuff.texFont, 0u);

    stuff.program.update(0, Vec4F(colour));

    gl::DrawArrays(gl::TRIANGLES, 0, int(glyphs.size()) * 6);
}
