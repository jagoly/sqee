#include <text/text.hpp>

using namespace sq;

void sq::draw_text(TextHandles& _handles, Font::Ptr& _font, const string& _text, glm::vec2 pos, glm::vec2 vp) {
    gl::Enable(gl::BLEND);

    gl::UseProgram(_handles.prog);
    gl::ActiveTexture(gl::TEXTURE0);
    gl::BindTexture(gl::TEXTURE_2D, _handles.tex);
    gl::BindBuffer(gl::ARRAY_BUFFER, _handles.vbo);

    FT_GlyphSlot g = _font->face->glyph;

    float x = pos.x * 2.f - vp.x; float y = pos.y * 2.f - vp.y;

    for (const char* p = _text.c_str(); *p; p++) {
        FT_Load_Char(_font->face, *p, FT_LOAD_RENDER);

        gl::TexImage2D(gl::TEXTURE_2D, 0, gl::ALPHA, g->bitmap.width, g->bitmap.rows,
                       0, gl::ALPHA, gl::UNSIGNED_BYTE, g->bitmap.buffer);

        float x2 = float(x + g->bitmap_left) / vp.x;
        float y2 = float(y + g->bitmap_top) / vp.y;
        float w = float(g->bitmap.width) / vp.x*2;
        float h = float(g->bitmap.rows) / vp.y*2;

        float box[16] = {
            x2,   y2-h, 0, 1,
            x2+w, y2-h, 1, 1,
            x2,   y2,   0, 0,
            x2+w, y2,   1, 0
        };

        gl::BufferData(gl::ARRAY_BUFFER, 16 * sizeof(float), box, gl::STREAM_DRAW);
        gl::DrawArrays(gl::TRIANGLE_STRIP, 0, 4);

        x += (g->advance.x >> 6) *2;
        y += (g->advance.y >> 6) *2;
    }
    gl::UseProgram(0);
}


TextHandles::TextHandles() {
    prog = gl::CreateProgram();

    const char* vShadStr =
        "#version 330\n"
        "layout(location = 0) in vec4 V;\n"
        "out vec2 texcoord;\n"
        "void main() {\n"
        "    texcoord = V.zw;\n"
        "    gl_Position = vec4(V.xy, 0.f, 1.f);\n"
        "}\n";

    const char* fShadStr =
        "#version 330\n"
        "in vec2 texcoord;\n"
        "uniform sampler2D tex;\n"
        "out vec4 fragColour;\n"
        "void main() {\n"
        "    fragColour = vec4(1, 1, 1, texture(tex, texcoord).a);\n"
        "}\n";

    GLuint vShad = gl::CreateShader(gl::VERTEX_SHADER);
    GLuint fShad = gl::CreateShader(gl::FRAGMENT_SHADER);

    int cnt = 140;
    gl::ShaderSource(vShad, 1, &vShadStr, &cnt);
    gl::CompileShader(vShad);
    gl::AttachShader(prog, vShad);

    cnt = 148;
    gl::ShaderSource(fShad, 1, &fShadStr, &cnt);
    gl::CompileShader(fShad);
    gl::AttachShader(prog, fShad);

    gl::LinkProgram(prog);

    gl::UseProgram(prog);
    u_colour = gl::GetUniformLocation(prog, "colour");
    u_tex = gl::GetUniformLocation(prog, "tex");
    gl::Uniform1i(u_tex, 0);

    gl::GenBuffers(1, &vbo);
    gl::EnableVertexAttribArray(0);
    gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
    gl::VertexAttribPointer(0, 4, gl::FLOAT, false, 0, nullptr);

    gl::GenTextures(1, &tex);
    gl::BindTexture(gl::TEXTURE_2D, tex);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
    gl::PixelStorei(gl::UNPACK_ALIGNMENT, 1);
}
