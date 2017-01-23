#include <algorithm>

#include <sqee/assert.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>

#include <sqee/debug/Text.hpp>

using namespace sq;

//============================================================================//

extern "C" const uchar data_TextFont[256*256];
extern "C" const uchar data_TextGlow[256*256];

//============================================================================//

namespace { // anonymous

//============================================================================//

const char vertexShaderSource[] = R"glsl(

#version 420 core

const vec2 texcrds[6] =
{
    vec2(0, 1), vec2(0, 0), vec2(1, 1),
    vec2(1, 0), vec2(1, 1), vec2(0, 0)
};

layout(location=0) in vec3 V_glyph;

out vec3 texcrd;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    gl_Position = vec4(V_glyph.xy, 0.f, 1.f);
    texcrd.xy = texcrds[gl_VertexID % 6];
    texcrd.z = V_glyph.z;
}

)glsl";

//========================================================//

const char fragmentShaderSource[] = R"glsl(

#version 420 core

in vec3 texcrd;

uniform vec4 colour;

layout(binding=0) uniform sampler2DArray tex;

out vec4 fragColour;

void main()
{
    float texel = texture(tex, texcrd).r;
    fragColour = vec4(colour.rgb, texel * colour.a);
}

)glsl";

//============================================================================//

struct TextBasicStuff
{
    VertexArray vao;
    FixedBuffer vbo { gl::ARRAY_BUFFER };

    TextureArray2D TEX_Font { Texture::Format::R8_UN };
    TextureArray2D TEX_Glow { Texture::Format::R8_UN };

    Shader VS_Text { Shader::Stage::Vertex };
    Shader FS_Text { Shader::Stage::Fragment };

    //========================================================//

    TextBasicStuff()
    {
        TEX_Font.set_filter_mode(true);
        TEX_Glow.set_filter_mode(true);

        vbo.allocate_editable(2048u * 12u, nullptr);
        vao.set_vertex_buffer(vbo, 0u, 12u);

        vao.add_float_attribute(0u, 3u, gl::FLOAT, false, 0u);

        TEX_Font.allocate_storage({16u, 16u, 256u});
        TEX_Font.load_memory(data_TextFont);

        TEX_Glow.allocate_storage({16u, 16u, 256u});
        TEX_Glow.load_memory(data_TextGlow);

        FS_Text.add_uniform("colour"); // Vec4F

        VS_Text.load(vertexShaderSource);
        FS_Text.load(fragmentShaderSource);
    }
};

//============================================================================//

} // anonymous namespace

//============================================================================//

void sq::render_text_basic(const string& text, Vec2I flow, Vec2I align,
                           Vec2F scale, Vec4F colour, bool shadow)
{
    SQASSERT(flow.x == -1 || flow.x == +1, "invalid horizontal flow");
    SQASSERT(flow.y == -1 || flow.y == +1, "invalid vertical flow");
    SQASSERT(align.x == -1 || align.x == 0 || align.x == +1, "invalid horizontal align");
    SQASSERT(align.y == -1 || align.y == 0 || align.y == +1, "invalid vertical align");

    //========================================================//

    static auto& context = Context::get();

    static TextBasicStuff stuff;

    const Vec2F viewPort (context.get_ViewPort());

    const Vec2F size = Vec2F(scale) / viewPort;
    const Vec2F margin = Vec2F(8.f) / viewPort;

    const float maxWidth = 1.f - margin.x * 2.f;

    //========================================================//

    vector<string> lines = { "" };

    for (uint i = 0u; i < text.size(); i++)
    {
        if (text.at(i) == '\n') { lines.emplace_back(); continue; }
        if ((lines.back().size() + 1u) * size.x * 0.75f > maxWidth) lines.emplace_back();
        lines.back().push_back(text.at(i));
    }

    //========================================================//

    // if flow.x == align.x, then reverse rows
    if ((flow.x == -1 && align.x == -1) || (flow.x == +1 && align.x == +1))
        for (auto& line : lines) std::reverse(line.begin(), line.end());

    // if flow.y == align.y, then reverse columns
    if ((flow.y == -1 && align.y == -1) || (flow.y == +1 && align.y == +1))
        std::reverse(lines.begin(), lines.end());

    //========================================================//

    // text origin and per-character offset

    Vec2F origin = Vec2F { 0.5f, 0.5f };
    Vec2F offset = Vec2F { 0.75f, 1.f } * size;

    if (align.x == +1) origin.x = 1.f - margin.x - size.x;
    else if (align.x == -1) origin.x = margin.x;
    if (align.y == +1) origin.y = 1.f - margin.y - size.y;
    else if (align.y == -1) origin.y = margin.y;

    if (align.x == +1) offset.x = -offset.x;
    if (align.y == +1) offset.y = -offset.y;

    //========================================================//

    vector<array<Vec3F, 6>> glyphs;

    float currentY = origin.y;

    for (const string& line : lines)
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

    //========================================================//

    stuff.vbo.update(0u, glyphs.size() * 72u, glyphs.data());

    context.bind_shader_pipeline();

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Alpha);

    context.use_Shader_Vert(stuff.VS_Text);
    context.use_Shader_Frag(stuff.FS_Text);
    context.bind_VertexArray(stuff.vao);

    if (shadow == true)
    {
        context.bind_Texture(stuff.TEX_Glow, 0u);
        const Vec3F rgb(maths::dot(Vec3F(colour), 0.33f) < 0.5f);
        stuff.FS_Text.update<Vec4F>("colour", Vec4F(rgb, colour.a));
        gl::DrawArrays(gl::TRIANGLES, 0, glyphs.size() * 6u);
    }

    context.bind_Texture(stuff.TEX_Font, 0u);
    stuff.FS_Text.update<Vec4F>("colour", colour);
    gl::DrawArrays(gl::TRIANGLES, 0, glyphs.size() * 6u);
}
