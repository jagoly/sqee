#include "sqee/redist/gl_ext_3_3.hpp"
#include "sqee/text/text.hpp"

#include <iostream>

using namespace sq;

extern "C" const uchar data_TextFont[256*256];

void sq::draw_tiny_text(const string& _text, float _scale, Alignment _align, vec2 _pos, uvec2 _viewport) {
    static GLuint tex = 0u;
    static GLuint vbo = 0u;
    static GLuint vao = 0u;
    static GLuint prog = 0u;

    static bool first = true;
    if (first) { first = false;
        gl::GenTextures(1, &tex);
        gl::GenBuffers(1, &vbo);
        gl::GenVertexArrays(1, &vao);
        prog = gl::CreateProgram();

        gl::ActiveTexture(gl::TEXTURE0); gl::BindTexture(gl::TEXTURE_2D, tex);
        gl::TexImage2D(gl::TEXTURE_2D, 0, gl::R8, 256, 256, 0, gl::RED, gl::UNSIGNED_BYTE, data_TextFont);
        gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
        gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
        gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
        gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);

        const char vShadStr[] =
            """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
            "   #version 330                                               \n"
            "   in vec4 V_poscrd;                                          \n"
            "   out vec2 texcrd;                                           \n"
            "   void main() {                                              \n"
            "       texcrd = V_poscrd.zw;                                  \n"
            "       gl_Position = vec4(V_poscrd.xy, 0, 1);                 \n"
            "   }                                                          \n"
            """""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""";
        const char fShadStr[] =
            """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
            "   #version 330                                               \n"
            "   in vec2 texcrd;                                            \n"
            "   uniform sampler2D tex;                                     \n"
            "   out vec4 fragColour;                                       \n"
            "   void main() {                                              \n"
            "       fragColour = vec4(1,1,1, texture(tex, texcrd).r);      \n"
            "   }                                                          \n"
            """""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""";

        GLuint vShad = gl::CreateShader(gl::VERTEX_SHADER);
        GLuint fShad = gl::CreateShader(gl::FRAGMENT_SHADER);
        const char* vSrc = (const char*)vShadStr;
        const char* fSrc = (const char*)fShadStr;
        int vCnt = sizeof(vShadStr);
        int fCnt = sizeof(fShadStr);

        gl::ShaderSource(vShad, 1, &vSrc, &vCnt);
        gl::ShaderSource(fShad, 1, &fSrc, &fCnt);

        gl::CompileShader(fShad);
        gl::CompileShader(vShad);
        gl::AttachShader(prog, vShad);
        gl::AttachShader(prog, fShad);
        gl::LinkProgram(prog);
        gl::DeleteShader(vShad);
        gl::DeleteShader(fShad);

        gl::BindVertexArray(vao);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::VertexAttribPointer(0, 4, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(0);
    }

    vector<array<vec4, 6>> pcData;
    float charX = 14.f*_scale / _viewport.x, charY = 16.f*_scale / _viewport.y;

    if (_align == Alignment::BL) {
        float textX = (_pos.x / _viewport.x) * 2.f - 1.f,
              textY = (_pos.y / _viewport.y) * 2.f - 1.f;
        float crntX = textX, crntY = textY;
        for (const char& c : _text) {
            if (c == ' ') crntX += charX * 0.6f;
            else if (c == '\n') crntX = textX, crntY += charY;
            else {
                const float row = (c / 16u) / 16.f;
                const float col = (c % 16u) / 16.f;
                const float offset = 1.f / 16.f;

                vec4 BL(crntX,       crntY,       col,        row+offset);
                vec4 BR(crntX+charX, crntY,       col+offset, row+offset);
                vec4 TL(crntX,       crntY+charY, col,        row);
                vec4 TR(crntX+charX, crntY+charY, col+offset, row);

                pcData.emplace_back(); array<vec4, 6>& pc = pcData.back();
                pc[0] = BL; pc[1] = TR; pc[2] = TL; pc[3] = TR; pc[4] = BL; pc[5] = BR;
                crntX += charX*0.6f; if (crntX > 1.f-charX) crntX = textX, crntY += charY;
            }
        }
    }

    if (_align == Alignment::TL) {
        float textX = (_pos.x / _viewport.x) * 2.f - 1.f,
              textY = (1.f - (_pos.y / _viewport.y)) * 2.f - 1.f - charY;
        float crntX = textX, crntY = textY;
        for (const char& c : _text) {
            if (c == ' ') crntX += charX * 0.6f;
            else if (c == '\n') crntX = textX, crntY -= charY;
            else {
                const float row = (c / 16u) / 16.f;
                const float col = (c % 16u) / 16.f;
                const float offset = 1.f / 16.f;

                vec4 BL(crntX,       crntY,       col,        row+offset);
                vec4 BR(crntX+charX, crntY,       col+offset, row+offset);
                vec4 TL(crntX,       crntY+charY, col,        row);
                vec4 TR(crntX+charX, crntY+charY, col+offset, row);

                pcData.emplace_back(); array<vec4, 6>& pc = pcData.back();
                pc[0] = BL; pc[1] = TR; pc[2] = TL; pc[3] = TR; pc[4] = BL; pc[5] = BR;
                crntX += charX*0.6f; if (crntX > 1.f-charX) crntX = textX, crntY -= charY;
            }
        }
    }

    gl::BindVertexArray(vao); gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
    gl::ActiveTexture(gl::TEXTURE0); gl::BindTexture(gl::TEXTURE_2D, tex);
    gl::BufferData(gl::ARRAY_BUFFER, pcData.size()*96, pcData.data(), gl::DYNAMIC_DRAW);
    gl::Enable(gl::BLEND); gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
    gl::UseProgram(prog); gl::DrawArrays(gl::TRIANGLES, 0, pcData.size()*6);
    gl::BindVertexArray(0u); gl::UseProgram(0u);
}
