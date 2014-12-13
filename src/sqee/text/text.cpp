#include <unordered_map>

#include <text/text.hpp>

using namespace sq;

extern const uchar  ttTexture[16*1024] asm("_binary_tinytext_texture_bin_start");
extern const ushort ttIndices[2048*6]  asm("_binary_tinytext_indices_bin_start");
const std::unordered_map<char, uchar> charMap = {
    {'0',  0}, {'1',  1}, {'2',  2}, {'3',  3}, {'4',  4}, {'5',  5}, {'6',  6}, {'7',  7},
    {'8',  8}, {'9',  9}, {'a', 10}, {'b', 11}, {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15},
    {'g', 16}, {'h', 17}, {'i', 18}, {'j', 19}, {'k', 20}, {'l', 21}, {'m', 22}, {'n', 23},
    {'o', 24}, {'p', 25}, {'q', 26}, {'r', 27}, {'s', 28}, {'t', 29}, {'u', 30}, {'v', 31},
    {'w', 32}, {'x', 33}, {'y', 34}, {'z', 35}, {'.', 36}, {',', 37}, {'!', 38}, {'?', 39},
    {'+', 40}, {'-', 41}, {'/', 42}, {'*', 43}, {'@', 44}, {'#', 45}, {'$', 46}, {'%', 47},
    {'^', 48}, {'&', 49}, {'(', 50}, {')', 51}, {'[', 52}, {']', 53}, {'{', 54}, {'}', 55},
    {'_', 56}, {'~', 57}, {'<', 58}, {'>', 59}, {':', 60}, {';', 61}, {'"', 62}, {'\"',63}
};

void sq::draw_tiny_text(const string& _text, float _scale, Alignment _align,
                        glm::vec2 _pos,  glm::uvec2 _viewport) {
    static GLuint tex = 0;
    static GLuint prog = 0;
    static GLuint ibo = 0;
    static GLuint vao = 0;
    static GLuint vboPos = 0;
    static GLuint vboCrd = 0;

    static bool first = true;
    if (first) { first = false;
        gl::GenTextures(1, &tex);
        gl::ActiveTexture(gl::TEXTURE0);
        gl::BindTexture(gl::TEXTURE_2D_ARRAY, tex);
        gl::TexImage3D(gl::TEXTURE_2D_ARRAY, 0, gl::R8, 16, 16, 64, 0,
                       gl::RED, gl::UNSIGNED_BYTE, ttTexture);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
        gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);

        const char vShadStr[] =
            "#version 330\n"
            "layout(location = 0) in vec2 V_pos;\n"
            "layout(location = 1) in vec3 V_crd;\n"
            "out vec3 texcrd;\n"
            "void main() {\n"
            "    texcrd = V_crd;\n"
            "    gl_Position = vec4(V_pos, 0, 1);\n"
            "}\n";
        const char fShadStr[] =
            "#version 330\n"
            "in vec3 texcrd;\n"
            "uniform sampler2DArray tex;\n"
            "out vec4 fragColour;\n"
            "void main() {\n"
            "    fragColour = vec4(1, 1, 1, 1.f-texture(tex, texcrd).r);\n"
            "}\n";

        GLuint vShad = gl::CreateShader(gl::VERTEX_SHADER);
        GLuint fShad = gl::CreateShader(gl::FRAGMENT_SHADER);
        const char* vSrc = (const char*)vShadStr;
        const char* fSrc = (const char*)fShadStr;
        int vCnt = sizeof(vShadStr);
        int fCnt = sizeof(fShadStr);

        gl::ShaderSource(vShad, 1, &vSrc, &vCnt);
        gl::CompileShader(vShad);

        gl::ShaderSource(fShad, 1, &fSrc, &fCnt);
        gl::CompileShader(fShad);

        prog = gl::CreateProgram();
        gl::AttachShader(prog, vShad);
        gl::AttachShader(prog, fShad);
        gl::LinkProgram(prog);


        gl::GenBuffers(1, &ibo);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, sizeof(ttIndices), ttIndices, gl::STATIC_DRAW);

        gl::GenVertexArrays(1, &vao);
        gl::BindVertexArray(vao);
        gl::EnableVertexAttribArray(0);
        gl::EnableVertexAttribArray(1);

        gl::GenBuffers(1, &vboPos);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboPos);
        gl::VertexAttribPointer(0, 2, gl::FLOAT, false, 0, 0);
        gl::GenBuffers(1, &vboCrd);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboCrd);
        gl::VertexAttribPointer(1, 3, gl::UNSIGNED_BYTE, false, 0, 0);

    }

    const glm::vec2 charSize(16.f*_scale / _viewport.x, 16.f*_scale / _viewport.y);
    const glm::vec2 textPos(_pos.x / _viewport.x*2-1, _pos.y / _viewport.y*2-1);

    vector<array<glm::vec2, 4>> posData;
    vector<array<glm::u8vec3, 4>> crdData;

    glm::vec2 crntPos(textPos);
    for (const char& c : _text) {
        if (c == ' ') crntPos.x += charSize.x;
        else if (c == '\n') { crntPos.x = textPos.x; crntPos.y += charSize.y; }
        else {
            posData.emplace_back();
            crdData.emplace_back();
            array<glm::vec2, 4>& pos = posData.back();
            array<glm::u8vec3, 4>& crd = crdData.back();
            pos[0] = {crntPos.x, crntPos.y + charSize.y};
            pos[1] = crntPos + charSize;
            pos[2] = crntPos;
            pos[3] = {crntPos.x + charSize.x, crntPos.y};
            const uchar id = charMap.at(c);
            crd[0] = {0, 0, id};
            crd[1] = {1, 0, id};
            crd[2] = {0, 1, id};
            crd[3] = {1, 1, id};

            crntPos.x += charSize.x;
        }
    }

    gl::BindBuffer(gl::ARRAY_BUFFER, vboPos);
    gl::BufferData(gl::ARRAY_BUFFER, posData.size()*8*4, posData.data(), gl::DYNAMIC_DRAW);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboCrd);
    gl::BufferData(gl::ARRAY_BUFFER, crdData.size()*12, crdData.data(), gl::DYNAMIC_DRAW);

    gl::Enable(gl::BLEND);
    gl::UseProgram(prog);
    gl::ActiveTexture(gl::TEXTURE0);
    gl::BindTexture(gl::TEXTURE_2D_ARRAY, tex);
    gl::BindVertexArray(vao);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::DrawElements(gl::TRIANGLES, posData.size()*6, gl::UNSIGNED_SHORT, 0);
    gl::UseProgram(0);
}
