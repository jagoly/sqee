#include <algorithm>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/text/Text.hpp>

using namespace sq;

extern "C" const uchar data_TextFont[256*256];
extern "C" const uchar data_TextGlow[256*256];

const char vertShader[] = R"glsl(
#version 420 core
in vec2 V_pos; in vec3 V_texcrd; out vec3 texcrd;
void main() { texcrd = V_texcrd; gl_Position = vec4(V_pos, 0, 1); }
)glsl";

const char fragShader[] = R"glsl(
#version 420 core
uniform float alpha; uniform vec3 colour;
uniform sampler2DArray tex; in vec3 texcrd; out vec4 fragColour;
void main() { fragColour = vec4(colour, texture(tex, texcrd).r) * alpha; }
)glsl";

struct PTCDat { float px, py, tx, ty, num; };

void sq::render_text_basic(const string& _text, Vec2U _view,
                           TextBasicFlow _hFlow, TextBasicFlow _vFlow,
                           TextBasicAlign _hAlign, TextBasicAlign _vAlign,
                           Vec2F _scale, Vec3F _colour, float _alpha, bool _shadow) {

    static auto& context = Context::get();

    static GLuint vbo = 0u;
    static GLuint vao = 0u;
    static GLuint texG = 0u;
    static GLuint texF = 0u;
    static GLuint prog = 0u;

    using Flow = TextBasicFlow;
    using Align = TextBasicAlign;

    static bool first = true;
    if (first) { first = false;
        gl::GenBuffers(1, &vbo);
        gl::GenVertexArrays(1, &vao);
        gl::GenTextures(1, &texG);
        gl::GenTextures(1, &texF);
        prog = gl::CreateProgram();

        gl::BindTexture(gl::TEXTURE_2D_ARRAY, texG);
        gl::TexStorage3D(gl::TEXTURE_2D_ARRAY, 1, gl::R8, 16, 16, 256);
        gl::TexSubImage3D(gl::TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, 256,
                          gl::RED, gl::UNSIGNED_BYTE, data_TextGlow);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_MAG_FILTER, gl::LINEAR);

        gl::BindTexture(gl::TEXTURE_2D_ARRAY, texF);
        gl::TexStorage3D(gl::TEXTURE_2D_ARRAY, 1, gl::R8, 16, 16, 256);
        gl::TexSubImage3D(gl::TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, 256,
                          gl::RED, gl::UNSIGNED_BYTE, data_TextFont);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_MAG_FILTER, gl::LINEAR);

        GLuint vShad = gl::CreateShader(gl::VERTEX_SHADER);
        GLuint fShad = gl::CreateShader(gl::FRAGMENT_SHADER);
        const char* vSrc = (const char*)vertShader;
        const char* fSrc = (const char*)fragShader;
        int vCnt = sizeof(vertShader);
        int fCnt = sizeof(fragShader);

        gl::ShaderSource(vShad, 1, &vSrc, &vCnt);
        gl::ShaderSource(fShad, 1, &fSrc, &fCnt);
        gl::CompileShader(vShad);
        gl::CompileShader(fShad);

        gl::AttachShader(prog, vShad);
        gl::AttachShader(prog, fShad);
        gl::LinkProgram(prog);
        gl::DeleteShader(vShad);
        gl::DeleteShader(fShad);

        gl::BindVertexArray(vao);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::VertexAttribPointer(0, 2, gl::FLOAT, false, 20, nullptr);
        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 20, (void*)8);
        gl::EnableVertexAttribArray(1);
    }

    vector<array<PTCDat, 6>> ptcVec;
    float charX = _scale.x / _view.x;
    float charY = _scale.y / _view.y;
    float margX = 8.f / _view.x;
    float margY = 8.f / _view.y;

    vector<string> lines = {""};
    for (uint i = 0u; i < _text.size(); i++) {
        if (_text.at(i) == '\n') lines.emplace_back();
        else { if ((lines.back().size() + 1u) * charX * 0.75f
                   > 1.f - margX * 2.f) lines.emplace_back();
            lines.back().push_back(_text.at(i)); }
    }

    if ((_hFlow == Flow::Negative && _hAlign == Align::Negative) ||
        (_hFlow == Flow::Positive && _hAlign == Align::Positive))
        for (auto& s : lines) std::reverse(s.begin(), s.end());
    if ((_vFlow == Flow::Negative && _vAlign == Align::Negative) ||
        (_vFlow == Flow::Positive && _vAlign == Align::Positive))
        std::reverse(lines.begin(), lines.end());

    float origX = 0.5f, origY = 0.5f;
    if (_hAlign == Align::Positive) origX = 1.f - margX;
    else if (_hAlign == Align::Negative) origX = margX;
    if (_vAlign == Align::Positive) origY = 1.f - margY;
    else if (_vAlign == Align::Negative) origY = margY;

    float offsX = charX * 0.75f; float offsY = charY * 1.f;
    if (_hAlign == Align::Positive) origX -= charX, offsX *= -1.f;
    if (_vAlign == Align::Positive) origY -= charY, offsY *= -1.f;

    float crntY = origY;
    for (const string& line : lines) {
        float crntX = origX;
        for (const char& c : line) {
            PTCDat BL {(   crntX   )*2-1, (   crntY   )*2-1, 0, 1, float(c)};
            PTCDat BR {(crntX+charX)*2-1, (   crntY   )*2-1, 1, 1, float(c)};
            PTCDat TL {(   crntX   )*2-1, (crntY+charY)*2-1, 0, 0, float(c)};
            PTCDat TR {(crntX+charX)*2-1, (crntY+charY)*2-1, 1, 0, float(c)};
            ptcVec.emplace_back(); auto& ptc = ptcVec.back();
            ptc[0] = BL; ptc[1] = TR; ptc[2] = TL;
            ptc[3] = TR; ptc[4] = BL; ptc[5] = BR;
            crntX += offsX;
        } crntY += offsY;
    }

    gl::BindVertexArray(vao); gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
    gl::BufferData(gl::ARRAY_BUFFER, ptcVec.size()*120, ptcVec.data(), gl::DYNAMIC_DRAW);
    gl::ActiveTexture(gl::TEXTURE0);
    gl::UseProgram(prog); gl::Uniform1f(0, _alpha);

    context.set_state(Context::Cull_Face::Disable);
    context.set_state(Context::Depth_Test::Disable);
    context.set_state(Context::Stencil_Test::Disable);
    context.set_state(Context::Blend_Mode::Alpha);

    if (_shadow == true) {
        float colour = maths::dot(Vec3F(0.33f, 0.33f, 0.33f), _colour) < 0.5f;
        gl::BindTexture(gl::TEXTURE_2D_ARRAY, texG);
        gl::Uniform3f(1, colour, colour, colour);
        gl::DrawArrays(gl::TRIANGLES, 0, ptcVec.size()*6);
    }

    gl::BindTexture(gl::TEXTURE_2D_ARRAY, texF);
    gl::Uniform3f(1, _colour.x, _colour.y, _colour.z);
    gl::DrawArrays(gl::TRIANGLES, 0, ptcVec.size()*6);
    gl::BindVertexArray(0u); gl::UseProgram(0u);
}
