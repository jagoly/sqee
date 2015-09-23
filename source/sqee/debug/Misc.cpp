#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/debug/Misc.hpp>

using namespace sq;

// Texture2D Vertex Shader /////
const char vert2DShader[] = R"glsl(

#version 420 core

const vec2 V_pos[6] = {
    vec2(-1, -1), vec2(+1, -1), vec2(+1, +1),
    vec2(+1, +1), vec2(-1, +1), vec2(-1, -1)
};

const vec2 V_tcrd[6] = {
    vec2(0, 0), vec2(1, 0), vec2(1, 1),
    vec2(1, 1), vec2(0, 1), vec2(0, 0)
};

out vec2 texcrd;

void main() {
    texcrd = V_tcrd[gl_VertexID];
    gl_Position = vec4(V_pos[gl_VertexID], 0, 1);
}

)glsl";


// Texture2D Fragment Shader /////
const char frag2DShader[] = R"glsl(

#version 420 core

in vec2 texcrd;

uniform sampler2D tex;

out vec4 fragColour;

void main() {
    fragColour = texture(tex, texcrd);
}

)glsl";


// Texture2DArray Fragment Shader /////
const char frag2DArrayShader[] = R"glsl(

#version 420 core

in vec2 texcrd;

uniform sampler2DArray tex;

uniform uint layer;

out vec4 fragColour;

void main() {
    fragColour = texture(tex, vec3(texcrd, layer));
}

)glsl";


// TextureCube Vertex Shader /////
const char vertCubeShader[] = R"glsl(

#version 420 core

const vec2 V_pos[6] = {
    vec2(-1, -1), vec2(+1, -1), vec2(+1, +1),
    vec2(+1, +1), vec2(-1, +1), vec2(-1, -1)
};

const vec3 V_tcrd[36] = {
    vec3(+1, +1, -1), vec3(+1, -1, -1),
    vec3(+1, -1, +1), vec3(+1, -1, +1),
    vec3(+1, +1, +1), vec3(+1, +1, -1),
    vec3(-1, -1, -1), vec3(-1, +1, -1),
    vec3(-1, +1, +1), vec3(-1, +1, +1),
    vec3(-1, -1, +1), vec3(-1, -1, -1),
    vec3(-1, +1, -1), vec3(+1, +1, -1),
    vec3(+1, +1, +1), vec3(+1, +1, +1),
    vec3(-1, +1, +1), vec3(-1, +1, -1),
    vec3(+1, -1, -1), vec3(-1, -1, -1),
    vec3(-1, -1, +1), vec3(-1, -1, +1),
    vec3(+1, -1, +1), vec3(+1, -1, -1),
    vec3(-1, +1, +1), vec3(+1, +1, +1),
    vec3(+1, -1, +1), vec3(+1, -1, +1),
    vec3(-1, -1, +1), vec3(-1, +1, +1),
    vec3(-1, -1, -1), vec3(+1, -1, -1),
    vec3(+1, +1, -1), vec3(+1, +1, -1),
    vec3(-1, +1, -1), vec3(-1, -1, -1)
};

uniform uint face;

out vec3 texcrd;

void main() {
    texcrd = V_tcrd[face*6u + gl_VertexID];
    gl_Position = vec4(V_pos[gl_VertexID], 0, 1);
}

)glsl";


// TextureCube Fragment Shader /////
const char fragCubeShader[] = R"glsl(

#version 420 core

in vec3 texcrd;

uniform samplerCube tex;

out vec4 fragColour;

void main() {
    fragColour = texture(tex, texcrd);
}

)glsl";


// TextureCubeArray Fragment Shader /////
const char fragCubeArrayShader[] = R"glsl(

#version 420 core

in vec3 texcrd;

uniform samplerCube tex;

uniform uint layer;

out vec4 fragColour;

void main() {
    fragColour = texture(tex, vec4(texcrd, layer));
}

)glsl";


void sq::debug_draw_texture2D() {
    static GLuint vao = 0u;
    static GLuint prog = 0u;

    static bool first = true;
    if (first) { first = false;
        gl::GenVertexArrays(1, &vao);
        prog = gl::CreateProgram();

        GLuint vShad = gl::CreateShader(gl::VERTEX_SHADER);
        GLuint fShad = gl::CreateShader(gl::FRAGMENT_SHADER);
        const char* vSrc = (const char*)vert2DShader;
        const char* fSrc = (const char*)frag2DShader;
        int vCnt = sizeof(vert2DShader);
        int fCnt = sizeof(frag2DShader);

        gl::ShaderSource(vShad, 1, &vSrc, &vCnt);
        gl::ShaderSource(fShad, 1, &fSrc, &fCnt);
        gl::CompileShader(vShad);
        gl::CompileShader(fShad);

        gl::AttachShader(prog, vShad);
        gl::AttachShader(prog, fShad);
        gl::LinkProgram(prog);
        gl::DeleteShader(vShad);
        gl::DeleteShader(fShad);
    }

    gl::UseProgram(prog);
    gl::BindVertexArray(vao);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
    gl::BindVertexArray(0u);
    gl::UseProgram(0u);
}


void sq::debug_draw_texture2DArray(uint _layer) {
    static GLuint vao = 0u;
    static GLuint prog = 0u;
    static GLint ufLayer = 0;

    static bool first = true;
    if (first) { first = false;
        gl::GenVertexArrays(1, &vao);
        prog = gl::CreateProgram();

        GLuint vShad = gl::CreateShader(gl::VERTEX_SHADER);
        GLuint fShad = gl::CreateShader(gl::FRAGMENT_SHADER);
        const char* vSrc = (const char*)vert2DShader;
        const char* fSrc = (const char*)frag2DArrayShader;
        int vCnt = sizeof(vert2DShader);
        int fCnt = sizeof(frag2DArrayShader);

        gl::ShaderSource(vShad, 1, &vSrc, &vCnt);
        gl::ShaderSource(fShad, 1, &fSrc, &fCnt);
        gl::CompileShader(vShad);
        gl::CompileShader(fShad);
        int length = 0; char log[2048]; gl::GetShaderInfoLog(vShad, 2048, &length, log);
//        if (length > 0) { string logStr(log); logStr.erase(logStr.rfind('\n'));
//            log_error("Failed to compile shader from string\n%s", logStr); }

        gl::AttachShader(prog, vShad);
        gl::AttachShader(prog, fShad);
        gl::LinkProgram(prog);
        gl::DeleteShader(vShad);
        gl::DeleteShader(fShad);

        ufLayer = gl::GetUniformLocation(prog, "layer");
    }

    gl::UseProgram(prog);
    gl::BindVertexArray(vao);
    gl::Uniform1i(ufLayer, _layer);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
    gl::BindVertexArray(0u);
    gl::UseProgram(0u);
}


void sq::debug_draw_textureCube(uint _face) {
    static GLuint vao = 0u;
    static GLuint prog = 0u;
    static GLint ufFace = 0;

    static bool first = true;
    if (first) { first = false;
        gl::GenVertexArrays(1, &vao);
        prog = gl::CreateProgram();

        GLuint vShad = gl::CreateShader(gl::VERTEX_SHADER);
        GLuint fShad = gl::CreateShader(gl::FRAGMENT_SHADER);
        const char* vSrc = (const char*)vertCubeShader;
        const char* fSrc = (const char*)fragCubeShader;
        int vCnt = sizeof(vertCubeShader);
        int fCnt = sizeof(fragCubeShader);

        gl::ShaderSource(vShad, 1, &vSrc, &vCnt);
        gl::ShaderSource(fShad, 1, &fSrc, &fCnt);
        gl::CompileShader(vShad);
        gl::CompileShader(fShad);

        gl::AttachShader(prog, vShad);
        gl::AttachShader(prog, fShad);
        gl::LinkProgram(prog);
        gl::DeleteShader(vShad);
        gl::DeleteShader(fShad);

        ufFace = gl::GetUniformLocation(prog, "face");
    }

    gl::UseProgram(prog);
    gl::BindVertexArray(vao);
    gl::Uniform1i(ufFace, _face);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
    gl::BindVertexArray(0u);
    gl::UseProgram(0u);
}


void sq::debug_draw_textureCubeArray(uint _layer, uint _face) {
    static GLuint vao = 0u;
    static GLuint prog = 0u;
    static GLint ufFace = 0;
    static GLint ufLayer = 0;

    static bool first = true;
    if (first) { first = false;
        gl::GenVertexArrays(1, &vao);
        prog = gl::CreateProgram();

        GLuint vShad = gl::CreateShader(gl::VERTEX_SHADER);
        GLuint fShad = gl::CreateShader(gl::FRAGMENT_SHADER);
        const char* vSrc = (const char*)vertCubeShader;
        const char* fSrc = (const char*)fragCubeArrayShader;
        int vCnt = sizeof(vertCubeShader);
        int fCnt = sizeof(fragCubeArrayShader);

        gl::ShaderSource(vShad, 1, &vSrc, &vCnt);
        gl::ShaderSource(fShad, 1, &fSrc, &fCnt);
        gl::CompileShader(vShad);
        gl::CompileShader(fShad);

        gl::AttachShader(prog, vShad);
        gl::AttachShader(prog, fShad);
        gl::LinkProgram(prog);
        gl::DeleteShader(vShad);
        gl::DeleteShader(fShad);

        int length = 0; char log[2048]; gl::GetProgramInfoLog(prog, 2048, &length, log);
//        if (length > 0) { string logStr(log); logStr.erase(logStr.rfind('\n'));
//            log_error("Failed to compile shader from string\n%s", logStr); }

        ufFace = gl::GetUniformLocation(prog, "face");
        ufLayer = gl::GetUniformLocation(prog, "layer");
    }

    gl::UseProgram(prog);
    gl::BindVertexArray(vao);
    gl::Uniform1i(ufFace, _face);
    gl::Uniform1i(ufLayer, _layer);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
    gl::BindVertexArray(0u);
    gl::UseProgram(0u);
}
