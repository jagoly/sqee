#include <sqee/redist/gl_ext_4_1.hpp>
#include <sqee/debug/OpenGL.hpp>
#include <sqee/app/Logging.hpp>

using namespace sq;

void sq::debug_callback(GLenum _source, GLenum _type, GLuint _id, GLenum _severity,
                        GLsizei _length, const GLchar* _message, const void* _param) {
    string source;
    if (_source == gl::DEBUG_SOURCE_API)
        source = "API";
    else if (_source == gl::DEBUG_SOURCE_APPLICATION)
        source = "APPLICATION";
    else if (_source == gl::DEBUG_SOURCE_OTHER)
        source = "OTHER";
    else if (_source == gl::DEBUG_SOURCE_SHADER_COMPILER)
        source = "SHADER_COMPILER";
    else if (_source == gl::DEBUG_SOURCE_THIRD_PARTY)
        source = "THIRD_PARTY";
    else if (_source == gl::DEBUG_SOURCE_WINDOW_SYSTEM)
        source = "WINDOW_SYSTEM";

    string type;
    if (_type == gl::DEBUG_TYPE_DEPRECATED_BEHAVIOR)
        type = "DEPRECATED_BEHAVIOR";
    if (_type == gl::DEBUG_TYPE_ERROR)
        type = "ERROR";
    if (_type == gl::DEBUG_TYPE_MARKER)
        type = "MARKER";
    if (_type == gl::DEBUG_TYPE_OTHER)
        type = "OTHER";
    if (_type == gl::DEBUG_TYPE_PERFORMANCE)
        type = "PERFORMANCE";
    if (_type == gl::DEBUG_TYPE_POP_GROUP)
        type = "POP_GROUP";
    if (_type == gl::DEBUG_TYPE_PORTABILITY)
        type = "PORTABILITY";
    if (_type == gl::DEBUG_TYPE_PUSH_GROUP)
        type = "PUSH_GROUP";
    if (_type == gl::DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        type = "UNDEFINED_BEHAVIOR";

    string severity;
    if (_severity == gl::DEBUG_SEVERITY_HIGH)
        severity = "HIGH";
    if (_severity == gl::DEBUG_SEVERITY_LOW)
        severity = "LOW";
    if (_severity == gl::DEBUG_SEVERITY_MEDIUM)
        severity = "MEDIUM";
    if (_severity == gl::DEBUG_SEVERITY_NOTIFICATION)
        severity = "NOTIFICATION";

    tfm::format(std::cout, "%s OpenGL: Source %s | Severity %s | Type %s | ID %s\n%s",
                get_time_str(), source, severity, type, _id, _message);
    std::cout << std::endl;

    return;
}


void sq::debug_message(const string& _message) {
    gl::DebugMessageInsert(gl::DEBUG_SOURCE_APPLICATION, gl::DEBUG_TYPE_MARKER, 0,
                           gl::DEBUG_SEVERITY_NOTIFICATION, _message.size(), _message.c_str());
}


const char vert2DShader[] =
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" #version 330 core                                    \n"
" #extension GL_ARB_shading_language_420pack : require \n"
" const vec2 V_pos[6] = {                              \n"
"   vec2(-1, -1), vec2(+1, -1), vec2(+1, +1),          \n"
"   vec2(+1, +1), vec2(-1, +1), vec2(-1, -1) };        \n"
" const vec2 V_tcrd[6] = {                             \n"
"   vec2(0, 0), vec2(1, 0), vec2(1, 1),                \n"
"   vec2(1, 1), vec2(0, 1), vec2(0, 0) };              \n"
" out vec2 texcrd;                                     \n"
" void main() { texcrd = V_tcrd[gl_VertexID];          \n"
"     gl_Position = vec4(V_pos[gl_VertexID], 0, 1); }  \n"
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""";

const char frag2DShader[] =
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" #version 330 core                                    \n"
" #extension GL_ARB_shading_language_420pack : require \n"
" layout(binding=0) uniform sampler2D tex;             \n"
" in vec2 texcrd; out vec4 fragColour;                 \n"
" void main() { fragColour = texture(tex, texcrd); }   \n"
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""";

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


const char vertCubeShader[] =
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" #version 330 core                                    \n"
" #extension GL_ARB_shading_language_420pack : require \n"
" const vec2 V_pos[6] = {                              \n"
"   vec2(-1, -1), vec2(+1, -1), vec2(+1, +1),          \n"
"   vec2(+1, +1), vec2(-1, +1), vec2(-1, -1) };        \n"
" const vec3 V_tcrd[6*6] = {                           \n"
"   vec3(+1, +1, -1), vec3(+1, -1, -1),                \n"
"   vec3(+1, -1, +1), vec3(+1, -1, +1),                \n"
"   vec3(+1, +1, +1), vec3(+1, +1, -1),                \n"
"   vec3(-1, -1, -1), vec3(-1, +1, -1),                \n"
"   vec3(-1, +1, +1), vec3(-1, +1, +1),                \n"
"   vec3(-1, -1, +1), vec3(-1, -1, -1),                \n"
"   vec3(-1, +1, -1), vec3(+1, +1, -1),                \n"
"   vec3(+1, +1, +1), vec3(+1, +1, +1),                \n"
"   vec3(-1, +1, +1), vec3(-1, +1, -1),                \n"
"   vec3(+1, -1, -1), vec3(-1, -1, -1),                \n"
"   vec3(-1, -1, +1), vec3(-1, -1, +1),                \n"
"   vec3(+1, -1, +1), vec3(+1, -1, -1),                \n"
"   vec3(-1, +1, +1), vec3(+1, +1, +1),                \n"
"   vec3(+1, -1, +1), vec3(+1, -1, +1),                \n"
"   vec3(-1, -1, +1), vec3(-1, +1, +1),                \n"
"   vec3(-1, -1, -1), vec3(+1, -1, -1),                \n"
"   vec3(+1, +1, -1), vec3(+1, +1, -1),                \n"
"   vec3(-1, +1, -1), vec3(-1, -1, -1) };              \n"
" out vec3 texcrd; uniform int face;                   \n"
" void main() { texcrd = V_tcrd[face*6 + gl_VertexID]; \n"
"     gl_Position = vec4(V_pos[gl_VertexID], 0, 1); }  \n"
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""";

const char fragCubeShader[] =
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" #version 330 core                                    \n"
" #extension GL_ARB_shading_language_420pack : require \n"
" layout(binding=0) uniform samplerCube tex;           \n"
" in vec3 texcrd; out vec4 fragColour;                 \n"
" void main() { fragColour = texture(tex, texcrd); }   \n"
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""";

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
